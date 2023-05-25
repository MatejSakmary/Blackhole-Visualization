#pragma once

#include <daxa/daxa.inl>

struct Globals
{
    daxa_f32mat4x4 view;
    daxa_f32mat4x4 projection;

    daxa_f32vec3 min_values;
    daxa_f32vec3 max_values;

    daxa_f32 min_magnitude_threshold;
    daxa_f32 max_magnitude_threshold;
    daxa_f32 min_magnitude;
    daxa_f32 max_magnitude;

    daxa_f32 num_colors;
    daxa_u32 uniform_sampling_step;
    daxa_f32vec3 colors[5];
    daxa_f32 thresholds[5];

    daxa_u32 streamline_num;
    daxa_u32 streamline_steps;

    daxa_f32 flat_transparency_value;
    daxa_f32 mag_transparency_pow;
};

struct StreamLineEntry
{
    daxa_f32vec3 pos;
    daxa_f32 mag;
};

struct DataPoint
{
    daxa_f32vec3 val;
};

DAXA_ENABLE_BUFFER_PTR(Globals)
DAXA_ENABLE_BUFFER_PTR(StreamLineEntry)
DAXA_ENABLE_BUFFER_PTR(DataPoint)
