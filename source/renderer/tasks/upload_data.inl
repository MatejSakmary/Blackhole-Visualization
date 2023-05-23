#pragma once

#include <daxa/daxa.inl>
#include <daxa/utils/task_list.inl>

#include "../shared/shared.inl"

DAXA_INL_TASK_USE_BEGIN(UploadGlobalsTaskBase, DAXA_CBUFFER_SLOT0)
DAXA_INL_TASK_USE_BUFFER(_globals, daxa_BufferPtr(Globals), TRANSFER_WRITE)
DAXA_INL_TASK_USE_END()


DAXA_INL_TASK_USE_BEGIN(UploadDataTaskBase, DAXA_CBUFFER_SLOT0)
DAXA_INL_TASK_USE_BUFFER(_field_data_staging, daxa_BufferPtr(DataPoint), TRANSFER_READ)
DAXA_INL_TASK_USE_BUFFER(_field_data, daxa_BufferPtr(DataPoint), TRANSFER_WRITE)
DAXA_INL_TASK_USE_END()

#if __cplusplus
#include "../context.hpp"

struct UploadGlobalsTask : UploadGlobalsTaskBase
{
    Context *context = {};
    void callback(daxa::TaskInterface ti)
    {
        auto cmd_list = ti.get_command_list();
        
        auto staging_mem_res = ti.get_allocator().allocate(sizeof(Globals));
        if(!staging_mem_res.has_value()) { ; }
        auto staging_mem = staging_mem_res.value();

        memcpy(staging_mem.host_address, &context->buffers.globals_cpu, sizeof(Globals));
        cmd_list.copy_buffer_to_buffer({
            .src_buffer = ti.get_allocator().get_buffer(),
            .src_offset = staging_mem.buffer_offset,
            .dst_buffer = uses._globals.buffer(),
            .size = sizeof(Globals),
        });
    }
};

struct UploadDataTask : UploadDataTaskBase
{
    Context *context = {};
    void callback(daxa::TaskInterface ti)
    {
        auto cmd_list = ti.get_command_list();
        
        cmd_list.copy_buffer_to_buffer({
            .src_buffer = uses._field_data_staging.buffer(),
            .src_offset = 0,
            .dst_buffer = uses._field_data.buffer(),
            .size = context->device.info_buffer(uses._field_data_staging.buffer()).size,
        });
    }
};

#endif //cplusplus