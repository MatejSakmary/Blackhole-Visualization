#pragma once

#include <daxa/daxa.inl>
#include <daxa/utils/task_list.inl>

#include "../shared/shared.inl"

DAXA_INL_TASK_USE_BEGIN(ComputeStreamLinesTaskBase, DAXA_CBUFFER_SLOT0)
DAXA_INL_TASK_USE_BUFFER(_globals, daxa_BufferPtr(Globals), COMPUTE_SHADER_READ)
DAXA_INL_TASK_USE_BUFFER(_streamline_entries, daxa_BufferPtr(StreamLineEntry), COMPUTE_SHADER_WRITE)
DAXA_INL_TASK_USE_BUFFER(_field_data, daxa_BufferPtr(DataPoint), VERTEX_SHADER_READ)
DAXA_INL_TASK_USE_END()

#if __cplusplus
#include "../context.hpp"

inline auto get_compute_streamlines_pipeline() -> daxa::ComputePipelineCompileInfo {
    return {
        .shader_info = { .source = daxa::ShaderFile{"compute_streamlines.glsl"}},
        .name = "compute streamlines pipeline"
    };
}

struct ComputeStreamLinesTask : ComputeStreamLinesTaskBase
{
    Context *context = {};
    void callback(daxa::TaskInterface ti)
    {
        auto cmd_list = ti.get_command_list();

        cmd_list.set_constant_buffer(ti.uses.constant_buffer_set_info());
        cmd_list.set_pipeline(*(context->pipelines.compute_streamlines));
        cmd_list.dispatch((context->buffers.globals_cpu.streamline_num + 31)/32);

        context->main_task_list.conditionals.at(MainConditionals::GENERATE_STREAMLINES) = false;
    }
};
#endif //__cplusplus