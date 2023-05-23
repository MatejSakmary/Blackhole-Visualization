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
};

struct DataPoint
{
    daxa_f32vec3 val;
};

DAXA_ENABLE_BUFFER_PTR(Globals)
DAXA_ENABLE_BUFFER_PTR(DataPoint)
