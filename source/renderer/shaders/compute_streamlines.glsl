#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/compute_streamlines.inl"

layout (local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

u32 rng_state;

u32 rand_pcg()
{
    u32 state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    u32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

void main()
{
    if(gl_GlobalInvocationID.x > deref(_globals).streamline_num)
    {
        return;
    }
    rng_state = gl_GlobalInvocationID.x;

    // u32vec3 start_pos = u32vec3(rand_pcg() % 512, rand_pcg % 512, rand_pcg % 512);
    u32vec3 start_pos = u32vec3(gl_GlobalInvocationID.x, 0, 0);

    u32 write_index = gl_GlobalInvocationID.x * deref(_globals).streamline_num;
    for(u32 i = 0; i < deref(_globals).streamline_steps; i++)
    {
        deref(_streamline_entries[write_index]) = StreamLineEntry(
            f32vec3(gl_GlobalInvocationID.x / 512.0, i / 512.0, 0.0),
            f32(i / 512.0)
        );
        write_index += 1;
    }
}