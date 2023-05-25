#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/draw_streamline_start_volume.inl"

DAXA_USE_PUSH_CONSTANT(DrawStreamlineStartVolumePC, pc)

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX

const f32vec3 vertices[8] = f32vec3[](
    f32vec3( 0.0,  1.0, 0.0 ),
    f32vec3( 0.0,  0.0, 0.0 ),
    f32vec3( 1.0,  0.0, 0.0 ),
    f32vec3( 1.0,  1.0, 0.0 ),

    f32vec3( 1.0,  1.0, 1.0 ),
    f32vec3( 0.0,  1.0, 1.0 ),
    f32vec3( 0.0,  0.0, 1.0 ),
    f32vec3( 1.0,  0.0, 1.0 )
);

const u32 indices[16] = u32[](
    0, 1, 2, 3, 4, 5, 0, 3,
    6, 5, 4, 7, 2, 1, 6, 7
);

void main()
{
    u32 idx = gl_InstanceIndex * 8 + gl_VertexIndex;
    f32vec4 origin_pos = f32vec4(vertices[indices[idx]], 1.0);
    f32vec3 pos = pc.position;
    f32vec3 scale = pc.scale;
    f32mat4x4 m_model = f32mat4x4(
        f32vec4( scale.x,  0.0,     0.0,    0.0), // first column
        f32vec4(   0.0,   scale.y,  0.0,    0.0), // second column
        f32vec4(   0.0,     0.0,   scale.z, 0.0), // third column
        f32vec4(  pos.x,   pos.y,   pos.z,  1.0)  // fourth column
    );

    gl_Position = deref(_globals).projection * deref(_globals).view * m_model * origin_pos;
}

#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT

layout (location = 0) out f32vec4 out_color;

void main()
{
    out_color = f32vec4(1.0, 1.0, 1.0, 1.0);
}
#endif //DAXA_SHADER_STAGE