#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <shared/shared.inl>
#include "tasks/compute_streamlines.inl"
#extension GL_EXT_debug_printf : enable

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
    if(gl_GlobalInvocationID.x >= deref(_globals).streamline_num)
    {
        return;
    }
    rng_state = gl_GlobalInvocationID.x;

    u32vec3 start_index = u32vec3(rand_pcg() % 512, rand_pcg() % 512, rand_pcg() % 512);

    f32vec3 min_bounds = deref(_globals).min_values;
    f32vec3 max_bounds = deref(_globals).max_values;
    f32vec3 pos = f32vec3(
        mix(min_bounds.x, max_bounds.x, start_index.x/512.0),
        mix(min_bounds.y, max_bounds.y, start_index.y/512.0),
        mix(min_bounds.z, max_bounds.z, start_index.z/512.0)
    );

    u32 translated_index = start_index.x + 512 * start_index.y + 512 * 512 * start_index.z;

    f32vec3 value = deref(_field_data[translated_index]).val;
    f32 magnitude = length(value); 

    u32 write_index = gl_GlobalInvocationID.x * deref(_globals).streamline_steps;
    bool out_of_bouds = false;
    for(u32 i = 0; i < deref(_globals).streamline_steps; i++)
    {
        // debugPrintfEXT("streamline id %d streamline step %d\n", gl_GlobalInvocationID.x, i);
        deref(_streamline_entries[write_index]) = StreamLineEntry(pos, magnitude);
        if(out_of_bouds)
        {
            write_index += 1;
            continue;
        }
        pos += value * 0.1;

        i32vec3 translated_pos = i32vec3(((pos - min_bounds) / (max_bounds - min_bounds)) * 512);
        if(any(lessThan(translated_pos,i32vec3(0, 0, 0))) || any(greaterThan(translated_pos,f32vec3(511, 511, 511))))
        {
            out_of_bouds = true;
        }

        translated_index = 
            clamp(u32(translated_pos.x), 0, 511) +
            clamp(u32(translated_pos.y), 0, 511) * 512 +
            clamp(u32(translated_pos.z), 0, 511) * 512 * 512;
            
        value = deref(_field_data[translated_index]).val;

        magnitude = length(value); 

        write_index += 1;
    }
}