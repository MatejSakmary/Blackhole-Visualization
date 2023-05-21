#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX
void main()
{
    gl_Position = f32vec4(0.0, 0.0, 0.0, 1.0);
}
#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT
layout (location = 0) out f32vec4 out_color;

void main()
{
    out_color = f32vec4(1.0, 0.0, 0.0, 1.0);
}
#endif