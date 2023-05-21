#pragma once

#include <daxa/daxa.inl>

struct Globals
{
    daxa_f32mat4x4 view;
    daxa_f32mat4x4 projection;
    daxa_f32 time;
};

DAXA_ENABLE_BUFFER_PTR(Globals)
