#pragma once

#include <array>
#include <daxa/types.hpp>
#include "renderer/context.hpp"
using namespace daxa::types;

struct GuiState
{
    static constexpr u32 max_colors = 5;
    bool random_sampling = true;
    bool use_transparency = true;
    bool flat_transparency = false;
    bool magnitude_transparency = true;

    u32 num_gradient_colors = 3;
    f32vec3 colors [max_colors] = {};
    f32 gradient_thresholds[max_colors] = {};

    u32 num_samples = 10000;
    f32vec2 min_max_magnitude = {};
    f32 min_magnitude_threshold = 0.0f;
    f32 flat_transparency_value = 0.15f;
    f32 mag_transparency_pow = 2.0f;
};