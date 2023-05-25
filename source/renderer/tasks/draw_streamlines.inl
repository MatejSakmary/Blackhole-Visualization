#pragma once

#include <daxa/daxa.inl>
#include <daxa/utils/task_list.inl>

#include "../shared/shared.inl"

DAXA_INL_TASK_USE_BEGIN(DrawStreamLineTaskBase, DAXA_CBUFFER_SLOT0)
DAXA_INL_TASK_USE_BUFFER(_globals, daxa_BufferPtr(Globals), VERTEX_SHADER_READ)
DAXA_INL_TASK_USE_BUFFER(_streamline_entries, daxa_BufferPtr(StreamLineEntry), VERTEX_SHADER_READ)
DAXA_INL_TASK_USE_IMAGE(_swapchain, daxa_Image2Du32, COLOR_ATTACHMENT)
DAXA_INL_TASK_USE_IMAGE(_depth, daxa_Image2Df32, DEPTH_ATTACHMENT)
DAXA_INL_TASK_USE_END()

#if __cplusplus
#include "../context.hpp"

inline auto get_draw_streamline_pipeline(Context const & context) -> daxa::RasterPipelineCompileInfo {
    return {
        .vertex_shader_info = daxa::ShaderCompileInfo { .source = daxa::ShaderFile{"draw_streamlines.glsl"}, },
        .fragment_shader_info = daxa::ShaderCompileInfo { .source = daxa::ShaderFile{"draw_streamlines.glsl"}, },
        .color_attachments = {{
            .format = context.swapchain.get_format(),
            .blend = {
                .blend_enable = context.use_transparency,
                .src_color_blend_factor = daxa::BlendFactor::SRC_ALPHA,
                .dst_color_blend_factor = daxa::BlendFactor::ONE,
                .color_blend_op = daxa::BlendOp::ADD,
                .src_alpha_blend_factor = daxa::BlendFactor::ONE,
                .dst_alpha_blend_factor = daxa::BlendFactor::ONE,
                .alpha_blend_op = daxa::BlendOp::ADD
            }
        }},
        .depth_test = {
            .depth_attachment_format = daxa::Format::D32_SFLOAT,
            .enable_depth_test = true,
            .enable_depth_write = true,
        },
        .raster = {
            .primitive_topology = daxa::PrimitiveTopology::LINE_STRIP,
            .primitive_restart_enable = false,
            .polygon_mode = daxa::PolygonMode::LINE,
            .face_culling = daxa::FaceCullFlagBits::BACK_BIT,
        },
        .name = "draw streamline pipeline"
    };
}

struct DrawStreamLineTask : DrawStreamLineTaskBase
{
    Context *context = {};
    void callback(daxa::TaskInterface ti)
    {
        auto cmd_list = ti.get_command_list();

        auto dimensions = context->swapchain.get_surface_extent();

        cmd_list.begin_renderpass({
            .color_attachments = 
            {{
                .image_view = {uses._swapchain.view()},
                .load_op = context->main_task_list.conditionals.at(MainConditionals::DRAW_FIELD) ? 
                    daxa::AttachmentLoadOp::LOAD : daxa::AttachmentLoadOp::CLEAR,
                .store_op = daxa::AttachmentStoreOp::STORE,
                .clear_value = std::array<f32, 4>{0.00f, 0.00f, 0.00f, 1.0f}
            }},
            .depth_attachment = 
            {{
                .image_view = {uses._depth.view()},
                .layout = daxa::ImageLayout::ATTACHMENT_OPTIMAL,
                .load_op = context->main_task_list.conditionals.at(MainConditionals::DRAW_FIELD) ? 
                    daxa::AttachmentLoadOp::LOAD : daxa::AttachmentLoadOp::CLEAR,
                .store_op = daxa::AttachmentStoreOp::STORE, 
                .clear_value = daxa::ClearValue{daxa::DepthValue{1.0f, 0}},
            }},
            .render_area = {.x = 0, .y = 0, .width = dimensions.x, .height = dimensions.y}
        });

        cmd_list.set_constant_buffer(ti.uses.constant_buffer_set_info());

        cmd_list.set_pipeline(*(context->pipelines.draw_streamlines));
        cmd_list.draw({
            .vertex_count = context->buffers.globals_cpu.streamline_steps,
            .instance_count = context->buffers.globals_cpu.streamline_num,
            .first_vertex = 0,
            .first_instance = 0
        });
        cmd_list.end_renderpass();
    }
};
#endif // __cplusplus