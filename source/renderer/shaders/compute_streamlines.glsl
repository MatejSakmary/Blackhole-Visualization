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
    rng_state = gl_GlobalInvocationID.x + 1235;

    u32vec3 rand_vec = u32vec3(rand_pcg(), rand_pcg(), rand_pcg());
    f32vec3 start_index = f32vec3(rand_vec) / f32(u32(-1));

    f32vec3 min_bounds = deref(_globals).stream_bb_min;
    f32vec3 max_bounds = deref(_globals).stream_bb_max;
    f32vec3 real_min_bounds = deref(_globals).min_values;
    f32vec3 real_max_bounds = deref(_globals).max_values;
    f32vec3 pos = f32vec3(
        mix(min_bounds.x, max_bounds.x, start_index.x),
        mix(min_bounds.y, max_bounds.y, start_index.y),
        mix(min_bounds.z, max_bounds.z, start_index.z)
    );

    // debugPrintfEXT("rand index %u, %u, %u\n", rand_vec.x, rand_vec.y, rand_vec.z);
    // debugPrintfEXT("start_index %f, %f, %f\n", start_index.x, start_index.y, start_index.z);

    bool out_of_bouds = false;
    f32vec3 value;
    f32 magnitude;
    u32 translated_index;
    i32vec3 translated_pos = i32vec3(((pos - real_min_bounds) / (real_max_bounds - real_min_bounds)) * 512);
    if(any(lessThan(translated_pos,i32vec3(0, 0, 0))) || any(greaterThan(translated_pos,f32vec3(511, 511, 511))))
    {
        out_of_bouds = true;
        pos = f32vec3(-100000.0, -100000.0, -100000.0);
        magnitude = 0.0;
    }else{
        translated_index = 
            clamp(u32(translated_pos.x), 0, 511) +
            clamp(u32(translated_pos.y), 0, 511) * 512 +
            clamp(u32(translated_pos.z), 0, 511) * 512 * 512;
        value = deref(_field_data[translated_index]).val;
        magnitude = length(value); 
    }

    u32 write_index = gl_GlobalInvocationID.x * deref(_globals).streamline_steps;
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

        i32vec3 translated_pos = i32vec3(((pos - real_min_bounds) / (real_max_bounds - real_min_bounds)) * 512);
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