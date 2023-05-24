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
        daxa::TaskBuffer field_data;
        daxa::TaskBuffer field_data_staging;
    };

    struct Images
    {
        daxa::TaskImage swapchain;
    };

    struct Pipelines
    {
        std::shared_ptr<daxa::RasterPipeline> draw_field;
    };

    struct MainTaskList
    {
        enum Conditionals 
        {
            UPLOAD_DATA = 0,
            COUNT
        };

        struct TransientBuffers
        {
            // daxa::TaskBufferHandle transient_task_buffer;
        };

        struct TransientImages
        {
            daxa::TaskImageHandle depth_buffer = {};
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
    u32 sample_count;

    bool random_sampling = true;
    bool use_transparency = true;
    bool flat_transparency = false;
    bool inside_interval = true;

    Buffers buffers;
    Images images;

    MainTaskList main_task_list;
    Pipelines pipelines;
};

using MainConditionals = Context::MainTaskList::Conditionals;