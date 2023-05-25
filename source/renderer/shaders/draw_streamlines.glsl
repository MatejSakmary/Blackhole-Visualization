#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_streamlines.inl"

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX
layout (location = 0) out f32 magnitude;

void main()
{
    u32 streamline_buffer_offset = gl_InstanceIndex * deref(_globals).streamline_steps;
    u32 in_streamline_offset = gl_VertexIndex;
    u32 summed_index = streamline_buffer_offset + in_streamline_offset;
    f32vec3 origin_pos = deref(_streamline_entries[summed_index]).pos;
    magnitude = deref(_streamline_entries[summed_index]).mag;

    gl_Position = deref(_globals).projection * deref(_globals).view * f32vec4(origin_pos, 1.0);
}

#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT
layout (location = 0) in f32 magnitude;
layout (location = 0) out f32vec4 out_color;

void main()
{
    out_color = f32vec4(magnitude, magnitude, magnitude, 1.0);
}

#endif // DAXA_SHADER_STAGE_FRAGMENT