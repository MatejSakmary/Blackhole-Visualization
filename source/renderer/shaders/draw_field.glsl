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
    // if(magnitude < 0.8)
    // {
    //     gl_Position = f32vec4(10.0, 10.0, 10.0, 10.0);
    //     return;
    // }
    if(is_start)
    {
        gl_Position = deref(_globals).projection * deref(_globals).view * origin_pos;
    } else {
        gl_Position = deref(_globals).projection * deref(_globals).view * (origin_pos + (val * (100.0/(512.0))));
    }
}
#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT
layout (location = 0) in f32 magnitude;
layout (location = 0) out f32vec4 out_color;

void main()
{
    out_color = f32vec4(magnitude/10, 0.0, 0.0, 1.0);
    // out_color = f32vec4(1.0, 0.0, 0.0, 1.0);
}
#endif