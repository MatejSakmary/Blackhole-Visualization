#include "renderer.hpp"

#include <string>
#include <functional>

#include <imgui_impl_glfw.h>
#include <daxa/utils/imgui.hpp>

Renderer::Renderer(const AppWindow & window) :
    context { .daxa_context{daxa::create_context({.enable_validation = false})} }
{
    context.device = context.daxa_context.create_device({ .name = "Daxa device" });

    context.swapchain = context.device.create_swapchain({ 
        .native_window = window.get_native_handle(),
#if defined(_WIN32)
        .native_window_platform = daxa::NativeWindowPlatform::WIN32_API,
#elif defined(__linux__)
        .native_window_platform = daxa::NativeWindowPlatform::XLIB_API,
#endif
        .present_mode = daxa::PresentMode::FIFO,
        .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST | daxa::ImageUsageFlagBits::COLOR_ATTACHMENT,
        .name = "Swapchain",
    });

    context.pipeline_manager = daxa::PipelineManager({
        .device = context.device,
        .shader_compile_options = {
            .root_paths = { 
                DAXA_SHADER_INCLUDE_DIR,
                "source/renderer",
                "source/renderer/shaders",
                "shaders",
                "shared"
            },
            .language = daxa::ShaderLanguage::GLSL,
        },
        .name = "Pipeline Compiler",
    });

    context.main_task_list.conditionals.fill(false);
    
    context.pipelines.draw_field = context.pipeline_manager.add_raster_pipeline(get_draw_field_pipeline(context)).value();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.get_glfw_window_handle(), true);
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    context.imgui_renderer = daxa::ImGuiRenderer({
        .device = context.device,
        .format = context.swapchain.get_format(),
    });

    context.main_task_list.task_list = daxa::TaskList({
        .device = context.device,
        .swapchain = context.swapchain,
        .reorder_tasks = true,
        .use_split_barriers = true,
        .jit_compile_permutations = true,
        .permutation_condition_count = Context::MainTaskList::Conditionals::COUNT,
        .name = "main task list"
    });

    create_resolution_independent_resources();
    create_resolution_dependent_resources();
    record_main_tasklist();
}

void Renderer::resize()
{
    context.swapchain.resize();

    context.main_task_list.task_list = daxa::TaskList({
        .device = context.device,
        .swapchain = context.swapchain,
        .reorder_tasks = true,
        .use_split_barriers = true,
        .jit_compile_permutations = true,
        .permutation_condition_count = Context::MainTaskList::Conditionals::COUNT,
        .name = "main task list"
    });

    create_resolution_dependent_resources();
    record_main_tasklist();
}

void Renderer::update(const GuiState & state)
{
}

void Renderer::draw(const Camera & camera) 
{
    auto extent = context.swapchain.get_surface_extent();
    GetProjectionInfo info {
        .near_plane = 0.1f,
        .far_plane = 500.0f,
    };

    context.buffers.globals_cpu.view = camera.get_view_matrix();
    context.buffers.globals_cpu.projection = camera.get_projection_matrix(info);

    context.images.swapchain.set_images({std::array{context.swapchain.acquire_next_image()}});

    if(!context.device.is_id_valid(context.images.swapchain.get_state().images[0]))
    {
        DEBUG_OUT("[Renderer::draw()] Got empty image from swapchain");
        return;
    }

    context.main_task_list.task_list.execute({{
        context.main_task_list.conditionals.data(),
        context.main_task_list.conditionals.size()}
    });
    auto result = context.pipeline_manager.reload_all();
    if(result.has_value()) 
    {
        if (result.value().is_ok())
        {
            DEBUG_OUT("[Renderer::draw()] Shaders recompiled successfully");
        }
        else 
        {
            DEBUG_OUT(result.value().to_string());
        }
    } 

    if(context.main_task_list.conditionals.at(MainConditionals::UPLOAD_DATA) == true)
    {
        context.device.destroy_buffer(context.buffers.field_data_staging.get_state().buffers[0]);
        context.main_task_list.conditionals.at(MainConditionals::UPLOAD_DATA) = false;
    }
}

void Renderer::create_resolution_dependent_resources()
{
    auto extent = context.swapchain.get_surface_extent();

    context.main_task_list.transient_images.depth_buffer = context.main_task_list.task_list.create_transient_image({
        .format = daxa::Format::D32_SFLOAT,
        .aspect = daxa::ImageAspectFlagBits::DEPTH,
        .size = {extent.x, extent.y, 1},
        .name = "depth buffer"
    });
}

auto Renderer::get_field_data_staging_pointer(u32 size) -> DataPoint*
{
    context.buffers.field_data_staging.set_buffers({
        .buffers = std::array{
            context.device.create_buffer({
                .size = size,
                .allocate_info = daxa::AutoAllocInfo{daxa::MemoryFlagBits::HOST_ACCESS_RANDOM},
                .name = "field data staging buffer"
            })
        },
        .latest_access = daxa::Access{
            .stages = daxa::PipelineStageFlagBits::HOST,
            .type = daxa::AccessTypeFlagBits::READ
        }
    });

    context.buffers.field_data.set_buffers({
        .buffers = std::array{
            context.device.create_buffer({
                .size = size,
                .allocate_info = daxa::AutoAllocInfo{daxa::MemoryFlagBits::DEDICATED_MEMORY},
                .name = "field data buffer"
            })
        },
    });

    context.main_task_list.conditionals.at(Context::MainTaskList::Conditionals::UPLOAD_DATA) = true;
    return context.device.get_host_address_as<DataPoint>(context.buffers.field_data_staging.get_state().buffers[0]);
};

void Renderer::create_resolution_independent_resources()
{
    context.images.swapchain = daxa::TaskImage({
        .swapchain_image = true,
        .name = "persistent swapchain image"
    });

    context.buffers.field_data_staging = daxa::TaskBuffer({ .name = "field data staging task buffer"});
    context.buffers.field_data = daxa::TaskBuffer({ .name = "field data task buffer"});

    context.buffers.globals = daxa::TaskBuffer({
        .initial_buffers = {
            .buffers = std::array{
                context.device.create_buffer({
                    .size = sizeof(Globals),
                    .name = "globals gpu buffer"
                })
            },
        },
        .name = "globals task buffer",
    });
}

void Renderer::record_main_tasklist()
{
    auto & tl = context.main_task_list.task_list;
    tl.use_persistent_buffer(context.buffers.globals); 
    tl.use_persistent_buffer(context.buffers.field_data); 
    tl.use_persistent_buffer(context.buffers.field_data_staging); 
    tl.use_persistent_image(context.images.swapchain);
    /* ============================================================================================================ */
    /* ===============================================  TASKS  ==================================================== */
    /* ============================================================================================================ */
    tl.conditional({
        .condition_index = 0,
        .when_true = [&]()
        {
            tl.add_task(UploadDataTask{{
                    .uses = {
                        ._field_data_staging = context.buffers.field_data_staging.handle(),
                        ._field_data = context.buffers.field_data.handle()
                }},
                &context
            });
        }
    });

    tl.add_task(DrawFieldTask{{
        .uses = {
            ._field_data = context.buffers.field_data.handle(),
            ._swapchain = context.images.swapchain.handle(),
            ._depth = context.main_task_list.transient_images.depth_buffer.subslice(
                {.image_aspect = daxa::ImageAspectFlagBits::DEPTH}
            ),
        }},
        &context 
    });

    tl.add_task(ImGuiTask{{ 
        .uses = { ._swapchain = context.images.swapchain.handle(), }},
        &context
    });

    tl.submit({});
    tl.present({});
    tl.complete({});
}

Renderer::~Renderer()
{
    context.device.wait_idle();
    ImGui_ImplGlfw_Shutdown();
    context.device.destroy_buffer(context.buffers.globals.get_state().buffers[0]);
    context.device.collect_garbage();
}