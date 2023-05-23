#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_field.inl"
#extension GL_EXT_debug_printf : enable

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX
layout (location = 0) out f32 magnitude;
void main()
{
    bool is_start = (gl_VertexIndex & 1) == 0;
    u32 x_coord = u32((gl_VertexIndex / 2)               & ((1 << 9) - 1));
    u32 y_coord = u32((gl_VertexIndex / (2 * 512))       & ((1 << 9) - 1));
    u32 z_coord = u32( gl_VertexIndex / (2 * 512 * 512));
    u32 target_z_coord = 32;
    // z_coord = 33;

    f32 x_pos = mix(deref(_globals).min_values.x, deref(_globals).max_values.x, f32(x_coord)/512.0);
    f32 y_pos = mix(deref(_globals).min_values.y, deref(_globals).max_values.y, f32(y_coord)/512.0);
    f32 z_pos = mix(deref(_globals).min_values.z, deref(_globals).max_values.z, f32(z_coord)/512.0);

    f32vec4 origin_pos = f32vec4(x_pos, y_pos, z_pos, 1.0);
    origin_pos.xyz *= 20.0;


    f32vec4 val = f32vec4(deref(_field_data[u32(gl_VertexIndex / 2.0)]).val, 0.0); 
    // f32vec4 val = f32vec4(0.0, 0.0, 1.0, 0.0); 

    magnitude = length(val);
    if(is_start)
    {
        gl_Position = deref(_globals).projection * deref(_globals).view * origin_pos;
    } else {
        gl_Position = deref(_globals).projection * deref(_globals).view * (origin_pos + (val * (10.0/(512.0))));
    }
    // if(magnitude < 2.0)
    // {
    //     gl_Position = f32vec4(10.0, 10.0, 10.0, 1.0);
    // }

    // if(target_z_coord != z_coord)
    // {
    //     gl_Position = f32vec4(10.0, 10.0, 10.0, 1.0);
    // } else 
    // {
    //     if(x_coord == 123 && y_coord == 123)
    //     {
    //         debugPrintfEXT("x %d y %d z %d is_start %d\n", x_coord, y_coord, z_coord, is_start);
    //     }
    // }

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