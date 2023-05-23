#include "application.hpp"

#include <daxa/utils/imgui.hpp>
#include <imgui_impl_glfw.h>
#include "renderer/context.hpp"

#include <fstream>

void Application::mouse_callback(f64 x, f64 y)
{
    f32 x_offset;
    f32 y_offset;
    if(!state.first_input)
    {
        x_offset = f32(x) - state.last_mouse_pos.x;
        y_offset = f32(y) - state.last_mouse_pos.y;
    } else {
        x_offset = 0.0f;
        y_offset = 0.0f;
        state.first_input = false;
    }

    ImGuiIO & io = ImGui::GetIO();
    if(state.key_table.bits.LMB)
    {
        io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, true);
        if(!io.WantCaptureMouse)
        {
            state.last_mouse_pos = {f32(x), f32(y)};
            camera.rotate_on_mouse(x_offset, y_offset);
        }
    }else{
        state.first_input = true;
    }
}

void Application::mouse_scroll_callback(f64 x, f64 y)
{
    camera.zoom_on_scroll(y);
}

void Application::mouse_button_callback(i32 button, i32 action, i32 mods)
{
    if(action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        auto update_state = [](i32 action) -> unsigned int
        {
            if(action == GLFW_PRESS) return 1;
            return 0;
        };
        switch(button)
        {
            case GLFW_MOUSE_BUTTON_LEFT: state.key_table.bits.LMB = update_state(action); return;
            default: break;
        }
    }
}

void Application::window_resize_callback(i32 width, i32 height)
{
    state.minimized = (width == 0 || height == 0);
    if(!state.minimized)
    {
        renderer.resize(); 
        camera.aspect_ratio = f32(width) / f32(height);
    }
}

void Application::key_callback(i32 key, i32 code, i32 action, i32 mods)
{
    if(action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        auto update_state = [](i32 action) -> unsigned int
        {
            if(action == GLFW_PRESS) return 1;
            return 0;
        };

        switch (key)
        {
            case GLFW_KEY_W: state.key_table.bits.W = update_state(action); return;
            case GLFW_KEY_A: state.key_table.bits.A = update_state(action); return;
            case GLFW_KEY_S: state.key_table.bits.S = update_state(action); return;
            case GLFW_KEY_D: state.key_table.bits.D = update_state(action); return;
            case GLFW_KEY_Q: state.key_table.bits.Q = update_state(action); return;
            case GLFW_KEY_E: state.key_table.bits.E = update_state(action); return;
            case GLFW_KEY_SPACE: state.key_table.bits.SPACE = update_state(action); return;
            case GLFW_KEY_LEFT_SHIFT: state.key_table.bits.LEFT_SHIFT = update_state(action); return;
            case GLFW_KEY_LEFT_CONTROL: state.key_table.bits.CTRL = update_state(action); return;
            default: break;
        }
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        state.fly_cam = !state.fly_cam;
        if(state.fly_cam)
        {
            window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            state.first_input = true;
        } else {
            window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void update_input()
{

}

Application::Application() : 
    window(INIT_WINDOW_DIMENSIONS,
    WindowVTable {
        .mouse_pos_callback = [this](const f64 x, const f64 y)
            {this->mouse_callback(x, y);},
        .mouse_scroll_callback = [this](const f64 x, const f64 y)
            {this->mouse_scroll_callback(x, y);},
        .mouse_button_callback = [this](const i32 button, const i32 action, const i32 mods)
            {this->mouse_button_callback(button, action, mods);},
        .key_callback = [this](const i32 key, const i32 code, const i32 action, const i32 mods)
            {this->key_callback(key, code, action, mods);},
        .window_resized_callback = [this](const i32 width, const i32 height)
            {this->window_resize_callback(width, height);},
    }),
    state{ .minimized = false },
    renderer{window},
    camera{{
        .focus_point = {0.847, -1.997, 0.0},
        .up = {0.0, 0.0, 1.0}, 
        .aspect_ratio = f32(INIT_WINDOW_DIMENSIONS.x)/f32(INIT_WINDOW_DIMENSIONS.y),
        .fov = glm::radians(70.0f)
    }}
{
    load_data();
}

void Application::ui_update()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDocking  | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse   |
        ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove       |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    ImGui::End();

    ImGui::Begin("Camera info");
    auto camera_position = camera.get_camera_position();

    ImGui::Text("Camera position is \n\t x: %f \n\t y: %f \n\t z: %f", camera_position.x, camera_position.y, camera_position.z);
    u32 min = 100u;
    u32 max = 1'000'000u;
    ImGui::SliderScalar("number of samples", ImGuiDataType_U32, &state.gui_state.num_samples, &min, &max);
    ImGui::SliderFloat(
        "Min magnitude threshold",
        &state.gui_state.min_magnitude_threshold,
        state.gui_state.min_max_magnitude.x,
        state.gui_state.min_max_magnitude.y);
    ImGui::Checkbox("Use random sampling", &state.gui_state.random_sampling);
    ImGui::End();

    ImGui::Render();
}

void Application::process_input()
{
    f64 this_frame_time = glfwGetTime();
    state.delta_time =  this_frame_time - state.last_frame_time;
    state.last_frame_time = this_frame_time;

    if(state.key_table.data > 0 && state.fly_cam)
    {
        if(state.key_table.bits.W)      { camera.move_camera(state.delta_time, Direction::FORWARD);    }
        if(state.key_table.bits.A)      { camera.move_camera(state.delta_time, Direction::LEFT);       }
        if(state.key_table.bits.S)      { camera.move_camera(state.delta_time, Direction::BACK);       }
        if(state.key_table.bits.D)      { camera.move_camera(state.delta_time, Direction::RIGHT);      }
        if(state.key_table.bits.Q)      { camera.move_camera(state.delta_time, Direction::ROLL_LEFT);  }
        if(state.key_table.bits.E)      { camera.move_camera(state.delta_time, Direction::ROLL_RIGHT); }
        if(state.key_table.bits.CTRL)   { camera.move_camera(state.delta_time, Direction::DOWN);       }
        if(state.key_table.bits.SPACE)  { camera.move_camera(state.delta_time, Direction::UP);         }
    }
}

void Application::load_data()
{
    constexpr i64 data_size = 512 * 512 * 512;
    constexpr i64 file_size = data_size * sizeof(DataPoint);

    std::ifstream data_file("data/data_bin/el2.bin", std::ios_base::binary);
    std::ifstream sizes_file("data/data_bin/el2_min_max.txt");
    ASSERT_MSG(data_file, "[Application::load_data()] Unable to open data file");
    ASSERT_MSG(sizes_file, "[Application::load_data()] Unable to open sizes file");

    std::string line;
    std::getline(sizes_file, line);

    f32vec3 min_size;
    f32vec3 max_size;
    f32 min_magnitude;
    f32 max_magnitue;
    char *next_token;
    char *token = strtok_s(const_cast<char*>(line.c_str()), " ", &next_token); 
    // Min max sizes of render area
    min_size.x = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    max_size.x = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    min_size.y = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    max_size.y = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    min_size.z = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    max_size.z = std::stof(std::string(token)); 

    // min max magnitudes
    token = strtok_s(nullptr, " ", &next_token); 
    min_magnitude = std::stof(std::string(token)); 
    token = strtok_s(nullptr, " ", &next_token); 
    max_magnitue = std::stof(std::string(token)); 

    sizes_file.close();
    renderer.set_field_size(min_size, max_size, min_magnitude, max_magnitue);
    state.gui_state.min_max_magnitude.x = min_magnitude;
    state.gui_state.min_max_magnitude.y = max_magnitue;

    data_file.read(reinterpret_cast<char*>(renderer.get_field_data_staging_pointer(file_size)), file_size);
    data_file.close();
}

void Application::main_loop()
{
    while (!window.get_window_should_close())
    {
        glfwPollEvents();
        process_input();
        ui_update();

        if (state.minimized) { continue; } 
    
        renderer.update(state.gui_state);
        renderer.draw(camera);
    }
}
