#include "camera.hpp"

#include <fstream>

auto constexpr static inline daxa_vec3_to_glm(f32vec3 vec) -> glm::vec3 { return glm::vec3(vec.x, vec.y, vec.z); }

Camera::Camera(const CameraInfo & info) : 
    focus_point{daxa_vec3_to_glm(info.focus_point)},
    up{daxa_vec3_to_glm(info.up)},
    aspect_ratio{info.aspect_ratio},
    fov{info.fov},
    speed{1.0f},
    pitch{0.0f},
    sensitivity{0.08f},
    zoom_sensitivity{0.7f},
    rotate_sensitivity{1.0f},
    roll_sensitivity{20.0f},
    dist{10.0},
    horizontal_angle{0.0f},
    vertical_angle{0.0f}
{
    // TODO(msakmary) FIX THIS - add separate function
    rotate_on_mouse(0, 0);
}

void Camera::rotate_on_mouse(f32 x_offset, f32 y_offset)
{
    horizontal_angle -= x_offset * ((2 * glm::pi<f32>()) / 1920) * rotate_sensitivity;
    vertical_angle -= y_offset * ((2 * glm::pi<f32>()) / 1080) * rotate_sensitivity;
    vertical_angle = glm::min(glm::max(glm::pi<f32>() * 0.01f, vertical_angle), glm::pi<f32>() * 0.99f);
    position.x = glm::sin(vertical_angle) * glm::cos(horizontal_angle);
    position.y = glm::sin(vertical_angle) * glm::sin(horizontal_angle);
    position.z = glm::cos(vertical_angle);
    position *= dist;
    position += focus_point;
}

void Camera::zoom_on_scroll(f32 offset)
{
    dist -= offset * zoom_sensitivity;
    dist = glm::max(dist, 0.1f);
    // TODO(msakmary) FIX THIS - add separate function
    rotate_on_mouse(0.0f, 0.0f);
}

void Camera::move_camera(f32 delta_time, Direction direction)
{
    glm::vec3 front = glm::normalize(focus_point - position);
    switch (direction)
    {
    case Direction::FORWARD:
        position += front * speed * delta_time;
        break;
    case Direction::BACK:
        position -= front * speed * delta_time;
        break;
    case Direction::LEFT:
        position -= glm::normalize(glm::cross(front, up)) * speed * delta_time;
        break;
    case Direction::RIGHT:
        position += glm::normalize(glm::cross(front, up)) * speed * delta_time;
        break;
    case Direction::UP:
        position += glm::normalize(glm::cross(glm::cross(front,up), front)) * speed * delta_time;
        break;
    case Direction::DOWN:
        position -= glm::normalize(glm::cross(glm::cross(front,up), front)) * speed * delta_time;
        break;
    case Direction::ROLL_LEFT:
        up = glm::rotate(up, static_cast<f32>(glm::radians(-roll_sensitivity * delta_time)), front);
        break;
    case Direction::ROLL_RIGHT:
        up = glm::rotate(up, static_cast<f32>(glm::radians(roll_sensitivity * delta_time)), front);
        break;
    
    default:
        DEBUG_OUT("[Camera::move_camera()] Unknown enum value");
        break;
    }
}

void Camera::update_front_vector(f32 x_offset, f32 y_offset)
{

}

auto Camera::get_view_matrix() const -> f32mat4x4
{
    auto view_mat = glm::lookAt(position, focus_point , up);

    return mat_from_span<f32, 4, 4>(std::span<f32, 4 * 4>{ glm::value_ptr(view_mat), 4 * 4 });
}

auto Camera::get_projection_matrix(const GetProjectionInfo & info) const -> f32mat4x4
{
    auto proj_mat = glm::perspective(fov, aspect_ratio, info.near_plane, info.far_plane);
    /* GLM is using OpenGL standard where Y coordinate of the clip coordinates is inverted */
    proj_mat[1][1] *= -1;

    return mat_from_span<f32, 4, 4>(std::span<f32, 4 * 4>{ glm::value_ptr(proj_mat), 4 * 4 });
}

// cope because I use daxa types - TODO(msakmary) switch to glm types for all internals
auto Camera::get_inv_view_proj_matrix(const GetProjectionInfo & info) const -> f32mat4x4
{
    auto view_mat = glm::lookAt(position, focus_point, up);
    auto proj_mat = glm::perspective(fov, aspect_ratio, info.near_plane, info.far_plane);
    /* GLM is using OpenGL standard where Y coordinate of the clip coordinates is inverted */
    proj_mat[1][1] *= -1;

    auto inv_proj_view_mat = glm::inverse(proj_mat * view_mat);

    return mat_from_span<f32, 4, 4>(std::span<f32, 4 * 4>{ glm::value_ptr(inv_proj_view_mat), 4 * 4 });
}

auto Camera::get_camera_position() const -> f32vec3
{
    return f32vec3{position.x, position.y, position.z};
}