#pragma once

#include <unordered_map>

#include <daxa/daxa.hpp>
#include <daxa/utils/task_list.hpp>
#include <daxa/utils/pipeline_manager.hpp>

#include "../window.hpp"
#include "../utils.hpp"
#include "../gui_state.hpp"
#include "../camera.hpp"
#include "shared/shared.inl"
#include "context.hpp"

#include "tasks/upload_data.inl"
#include "tasks/draw_field.inl"

using namespace daxa::types;

struct Renderer
{
    explicit Renderer(const AppWindow & window);
    ~Renderer();

    void resize();
    void update(const GuiState & state);
    void draw(const Camera & camera);

    auto get_field_data_staging_pointer(u32 size) -> DataPoint*;

    private:
        Context context;

        void record_main_tasklist();
        void create_resolution_independent_resources();
        void create_resolution_dependent_resources();
};