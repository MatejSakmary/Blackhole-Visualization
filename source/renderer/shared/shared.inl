#pragma once

#include <daxa/daxa.inl>

struct Globals
{
    daxa_f32mat4x4 view;
    daxa_f32mat4x4 projection;
    daxa_f32 time;
};

struct DataPoint
{
    daxa_f32vec3 val;
};

DAXA_ENABLE_BUFFER_PTR(Globals)
DAXA_ENABLE_BUFFER_PTR(DataPoint)
