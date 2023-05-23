#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_field.inl"
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
    u32 x_coord = u32((gl_VertexIndex / 2)               & ((1 << 9) - 1));
    u32 y_coord = u32((gl_VertexIndex / (2 * 512))       & ((1 << 9) - 1));
    u32 z_coord = u32( gl_VertexIndex / (2 * 512 * 512));
    f32vec4 val = f32vec4(deref(_field_data[u32(gl_VertexIndex / 2.0)]).val, 0.0); 
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
    // origin_pos.xyz *= 20.0;

    magnitude = length(val);
    if(magnitude < deref(_globals).magnitude_threshold)
    {
        gl_Position = f32vec4(10.0, 10.0, 10.0, 10.0);
        return;
    }
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
    f32 interplocation_value = (magnitude - deref(_globals).min_magnitude) / deref(_globals).max_magnitude;
    f32vec3 color1 = urgb_to_frgb(u32vec3(255, 237, 160));
    f32vec3 color2 = urgb_to_frgb(u32vec3(254, 178, 76));
    f32vec3 color3 = urgb_to_frgb(u32vec3(240, 59, 32));
    f32vec3 final_color;
    if(interplocation_value < 0.333)
    {
        final_color = mix(color1, color2, interplocation_value * 3.0);
    } else {
        final_color = f32vec3(interplocation_value, interplocation_value, interplocation_value);
    }
    // out_color = f32vec4(final_color, max(pow((magnitude - 0.1)/2.0, 5), 0.01));
    // out_color = f32vec4(interplocation_value, interplocation_value, interplocation_value, max(pow(interplocation_value, 5), 0.2));
    // out_color = f32vec4(final_color, 0.15);
    // if(pow(interplocation_value, 2) < 0.05)
    // {
    //     discard;
    // }
    out_color = f32vec4(interplocation_value, interplocation_value, interplocation_value, interplocation_value);
}
#endif