#pragma once

#include <vector>
#include <array>
#include <string_view>
#include <daxa/daxa.hpp>
#include <daxa/utils/task_list.hpp>
#include <daxa/utils/imgui.hpp>
#include <daxa/utils/pipeline_manager.hpp>

#include "shared/shared.inl"

using namespace std::literals;

struct Context
{
    struct Buffers
    {
        Globals globals_cpu;
        daxa::TaskBuffer globals;
    };

    struct Images
    {
        daxa::TaskImage swapchain;
    };

    struct Pipelines
    {
    };

    struct MainTaskList
    {
        enum Conditionals 
        {
            CONDITIONAL = 0,
            COUNT
        };

        struct TransientBuffers
        {
            // daxa::TaskBufferHandle transient_task_buffer;
        };

        struct TransientImages
        {
            daxa::TaskImageHandle depth_buffer;
        };

        daxa::TaskList task_list;
        TransientBuffers transient_buffers;
        TransientImages transient_images;
        std::array<bool, Conditionals::COUNT> conditionals;
    };


    daxa::Context daxa_context;
    daxa::Device device;
    daxa::Swapchain swapchain;
    daxa::PipelineManager pipeline_manager;
    daxa::ImGuiRenderer imgui_renderer;

    daxa::SamplerId linear_sampler;

    Buffers buffers;
    Images images;

    MainTaskList main_task_list;
    Pipelines pipelines;
};