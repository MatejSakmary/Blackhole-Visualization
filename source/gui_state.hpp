#pragma once

#include <array>
#include <daxa/types.hpp>
#include "renderer/context.hpp"
using namespace daxa::types;

struct GuiState
{
    bool random_sampling = true;
    u32 num_samples = 10000;
    f32vec2 min_max_magnitude = {};
    f32 min_magnitude_threshold = 0.0f;
};