#pragma once

#include <daxa/daxa.inl>

struct Globals
{
    daxa_f32mat4x4 view;
    daxa_f32mat4x4 projection;

    daxa_f32vec3 min_values;
    daxa_f32vec3 max_values;

    daxa_f32 magnitude_threshold;
    daxa_f32 min_magnitude;
    daxa_f32 max_magnitude;

    daxa_f32 num_colors;
    daxa_f32vec3 colors[5];
    daxa_f32 thresholds[5];

    daxa_f32 flat_transparency_value;
    daxa_f32 mag_transparency_pow;
};

struct DataPoint
{
    daxa_f32vec3 val;
};

DAXA_ENABLE_BUFFER_PTR(Globals)
DAXA_ENABLE_BUFFER_PTR(DataPoint)
