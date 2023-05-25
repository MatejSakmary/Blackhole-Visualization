#pragma once

#include <array>
#include <daxa/types.hpp>
#include <daxa/utils/math_operators.hpp>
#include "renderer/context.hpp"

using namespace daxa::types;
using namespace daxa::math_operators;

struct GuiState
{
    static constexpr u32 max_colors = 5u;
    static constexpr u32 num_presets = 3u;
    static constexpr u32 max_streamline_entries = 4'000'000u;
    static constexpr u32 max_streamline_steps = 1'000u;

    bool random_sampling = true;
    bool use_transparency = true;
    bool flat_transparency = false;
    bool magnitude_transparency = true;
    bool view_inside_interval = true;

    bool draw_field = true;
    bool draw_streamlines = false;
    bool live_preview_streamlines = false;

    u32 num_gradient_colors = 3;
    f32vec3 colors [max_colors] = {};
    f32 gradient_thresholds[max_colors] = {};

    u32 num_samples = 10000;
    u32 uniform_sampling_step = 1000;
    f32vec2 min_max_magnitude = {};
    f32 min_magnitude_threshold = 0.0f;
    f32 max_magnitude_threshold = 0.0f;
    f32 flat_transparency_value = 0.15f;
    f32 mag_transparency_pow = 2.0f;

    // u32 streamline_num = 10000;
    // u32 streamline_steps = 1000;
    u32 streamline_num = 512;
    u32 streamline_steps = 512;

    static constexpr std::array<std::array<f32vec3, GuiState::max_colors>, GuiState::num_presets> presets{
        std::array<f32vec3, GuiState::max_colors>{
            f32vec3{189.0f , 0.0f , 38.0f}  / 255.0f,
            f32vec3{240.0f , 59.0f , 32.0f}  / 255.0f,
            f32vec3{253.0f , 141.0f, 60.0f}  / 255.0f,
            f32vec3{254.0f , 204.0f, 92.0f} / 255.0f,
            f32vec3{255.0f , 255.0f, 178.0f} / 255.0f,
        },
        std::array<f32vec3, GuiState::max_colors>{
            f32vec3{255.0f , 255.0f, 204.0f} / 255.0f,
            f32vec3{161.0f , 218.0f, 180.0f} / 255.0f,
            f32vec3{65.0f , 182.0f, 196.0f}  / 255.0f,
            f32vec3{44.0f , 127.0f , 184.0f}  / 255.0f,
            f32vec3{37.0f , 52.0f , 148.0f}  / 255.0f,
        },
        std::array<f32vec3, GuiState::max_colors>{
            f32vec3{253.0f , 225.0f, 179.0f} / 255.0f,
            f32vec3{253.0f , 197.0f, 123.0f} / 255.0f,
            f32vec3{252.0f , 141.0f, 89.0f}  / 255.0f,
            f32vec3{229.0f , 89.0f , 67.0f}  / 255.0f,
            f32vec3{255.0f , 79.0f , 79.0f}  / 255.0f,
        },
    };

    inline void load_color_preset(i32 index)
    {
        num_gradient_colors = 5;
        f32 threshold = 0;
        for(int i = 0; i < num_gradient_colors; i++)
        {
            threshold += 0.2;
            gradient_thresholds[i] = threshold;
        }
        std::copy(presets.at(index).begin(), presets.at(index).end(), colors);
    }
};
