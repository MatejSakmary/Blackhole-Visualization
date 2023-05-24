#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_field.inl"
#include "virtual_defines.glsl"
#extension GL_EXT_debug_printf : enable

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX
layout (location = 0) out f32 magnitude;

#if defined(RANDOM_SAMPLING)
u32 rng_state;

u32 rand_pcg()
{
    u32 state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    u32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}
#endif //RANDOM_SAMPLING

void main()
{
    bool is_start = (gl_VertexIndex & 1) == 0;
#if defined(GRID_SAMPLING)
    u32 step_mult_vert_idx = (gl_VertexIndex & (~1)) * deref(_globals).uniform_sampling_step;
    u32 x_coord = u32((step_mult_vert_idx / 2)               & ((1 << 9) - 1));
    u32 y_coord = u32((step_mult_vert_idx / (2 * 512))       & ((1 << 9) - 1));
    u32 z_coord = u32( step_mult_vert_idx / (2 * 512 * 512));
    f32vec4 val = f32vec4(deref(_field_data[u32(step_mult_vert_idx / 2.0)]).val, 0.0); 
    // debugPrintfEXT("x: %d ,y: %d, z: %d\n", x_coord, y_coord, z_coord);
    // f32vec4 val = f32vec4(0.0, 0.0, 1.0, 0.0);
#elif defined(RANDOM_SAMPLING)
    rng_state = gl_VertexIndex / 2;
    u32 x_coord = rand_pcg() % 512;
    u32 y_coord = rand_pcg() % 512;
    u32 z_coord = rand_pcg() % 512;
    u32 translated_index = x_coord + 512 * y_coord + 512 * 512 * z_coord;
    f32vec4 val = f32vec4(deref(_field_data[translated_index]).val, 0.0); 
#endif

    f32 x_pos = mix(deref(_globals).min_values.x, deref(_globals).max_values.x, f32(x_coord)/512.0);
    f32 y_pos = mix(deref(_globals).min_values.y, deref(_globals).max_values.y, f32(y_coord)/512.0);
    f32 z_pos = mix(deref(_globals).min_values.z, deref(_globals).max_values.z, f32(z_coord)/512.0);

    f32vec4 origin_pos = f32vec4(x_pos, y_pos, z_pos, 1.0);

    magnitude = length(val);
#if defined(INSIDE_INTERVAL)
    if(magnitude < deref(_globals).min_magnitude_threshold ||
       magnitude > deref(_globals).max_magnitude_threshold)
    {
        gl_Position = f32vec4(10.0, 10.0, 10.0, 10.0);
        return;
    }
#elif defined(OUTSIDE_INTERVAL)
    if(magnitude > deref(_globals).min_magnitude_threshold &&
       magnitude < deref(_globals).max_magnitude_threshold)
    {
        gl_Position = f32vec4(10.0, 10.0, 10.0, 10.0);
        return;
    }
#endif //OUTSIDE_INTERVAL

    if(is_start)
    {
        gl_Position = deref(_globals).projection * deref(_globals).view * origin_pos;
    } else {
        gl_Position = deref(_globals).projection * deref(_globals).view * (origin_pos + (val / (3 * deref(_globals).max_magnitude)));
    }
}
#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT
layout (location = 0) in f32 magnitude;
layout (location = 0) out f32vec4 out_color;

f32vec3 urgb_to_frgb(u32vec3 urgb)
{
    return f32vec3(urgb)/255.0;
}

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
    // out_color = f32vec4(final_color, max(pow((magnitude - 0.1)/2.0, 5), 0.01));
    // out_color = f32vec4(interplocation_value, interplocation_value, interplocation_value, max(pow(interplocation_value, 5), 0.2));
    // out_color = f32vec4(final_color, 0.15);
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