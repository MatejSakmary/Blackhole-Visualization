#pragma once

#include <daxa/daxa.inl>
#include <daxa/utils/task_list.inl>

#include "../shared/shared.inl"

DAXA_INL_TASK_USE_BEGIN(UploadDataTaskBase, DAXA_CBUFFER_SLOT0)
DAXA_INL_TASK_USE_BUFFER(_field_data_staging, daxa_BufferPtr(DataPoint), TRANSFER_READ)
DAXA_INL_TASK_USE_BUFFER(_field_data, daxa_BufferPtr(DataPoint), TRANSFER_WRITE)
DAXA_INL_TASK_USE_END()

#if __cplusplus
#include "../context.hpp"

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