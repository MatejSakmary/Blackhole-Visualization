#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_streamlines.inl"
#include "virtual_defines.glsl"

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
    f32 interpolation_value = (magnitude - deref(_globals).min_magnitude) / deref(_globals).max_magnitude;
    i32 index = 0;
    f32 lower_threshold = 0.0;
    f32 upper_threshold = 0.0;
    for(i32 i = 0; i < deref(_globals).num_colors; i++)
    {
        lower_threshold = upper_threshold;
        upper_threshold = deref(_globals).thresholds[i];
        if(interpolation_value < upper_threshold && interpolation_value > lower_threshold)
        {
            index = i;
            break;
        }
    }
    f32 range = upper_threshold - lower_threshold;
    f32 rescaled_interpolation_value = (interpolation_value - lower_threshold) / range;
    f32vec3 final_color = mix(deref(_globals).colors[max(index - 1, 0)], deref(_globals).colors[index], rescaled_interpolation_value);
#if defined(FLAT_TRANSPARENCY)
    out_color = f32vec4(final_color, deref(_globals).flat_transparency_value);
#else 
    out_color = f32vec4(final_color, pow(interpolation_value, deref(_globals).mag_transparency_pow));
#endif // FLAT_TRANSPARENCY
    if(out_color.w < 0.001)
    {
        discard;
    }
}

#endif // DAXA_SHADER_STAGE_FRAGMENT