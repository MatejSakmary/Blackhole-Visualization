#include "renderer.hpp"

#include <string>

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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.get_glfw_window_handle(), true);
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    context.imgui_renderer = daxa::ImGuiRenderer({
        .device = context.device,
        .format = context.swapchain.get_format(),
    });

    create_resolution_independent_resources();
    create_resolution_dependent_resources();
    create_main_tasklist();
}

void Renderer::resize()
{
    context.swapchain.resize();
    create_resolution_dependent_resources();
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
    if(result.value()) 
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

void Renderer::create_resolution_independent_resources()
{
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

void Renderer::create_main_tasklist()
{
    context.main_task_list.task_list = daxa::TaskList({
        .device = context.device,
        .swapchain = context.swapchain,
        .reorder_tasks = true,
        .use_split_barriers = true,
        .jit_compile_permutations = true,
        .permutation_condition_count = Context::MainTaskList::Conditionals::COUNT,
        .name = "main task list"
    });

    context.main_task_list.task_list.use_persistent_buffer(context.buffers.globals); 
    /* ============================================================================================================ */
    /* ===============================================  TASKS  ==================================================== */
    /* ============================================================================================================ */

    context.main_task_list.task_list.submit({});
    context.main_task_list.task_list.present({});
    context.main_task_list.task_list.complete({});
}

Renderer::~Renderer()
{
    context.device.wait_idle();
    ImGui_ImplGlfw_Shutdown();
    context.device.destroy_buffer(context.buffers.globals.get_state().buffers[0]);
    context.device.collect_garbage();
}