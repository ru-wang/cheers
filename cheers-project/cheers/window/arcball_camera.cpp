#include "cheers/window/arcball_camera.hpp"

#include <cmath>
#include <algorithm>

#include <glm/ext.hpp>

namespace cheers {

void ArcballCamera::UpdateMatrixView(const WindowUiState& ui_state) {
  const auto& mouse_action = ui_state.mouse_action;

  if (mouse_action.button_pressed.at(GLFW_MOUSE_BUTTON_LEFT)) {
    m_azimuth -= glm::radians(mouse_action.pos_delta.x) * ROTATION_RESOLUTION;
    m_elevation += glm::radians(mouse_action.pos_delta.y) * ROTATION_RESOLUTION;
    m_elevation = std::clamp(m_elevation, -ELEVATION_ABS_MAX, ELEVATION_ABS_MAX);
  } else if (mouse_action.button_pressed.at(GLFW_MOUSE_BUTTON_RIGHT)) {
    glm::vec3 view_dir = glm::rotate(-m_eye_direction, m_azimuth, m_up_vector);
    glm::vec3 elevation_axis = glm::cross(m_up_vector, view_dir);
    view_dir = glm::rotate(view_dir, m_elevation, elevation_axis);

    glm::vec3 azimuth_axis = glm::cross(view_dir, elevation_axis);
    glm::vec3 translation =
        mouse_action.pos_delta.x * elevation_axis + mouse_action.pos_delta.y * azimuth_axis;
    m_center_point += m_distance * translation * TRANSLATION_RESOLUTION;
  } else {
    float translation = -m_distance * mouse_action.wheel_delta.y * TRANSLATION_Z_RESOLUTION;
    m_distance = std::max(m_distance + translation, TRANSLATION_Z_MIN);
  }
}

void ArcballCamera::UpdateMatrixProjection(const WindowUiState& ui_state) {
  m_aspect = ui_state.frame_buffer.x * 1.0f / ui_state.frame_buffer.y;
}

glm::mat4 ArcballCamera::GetMatrixView() const {
  glm::vec3 eye_vector = glm::rotate(m_eye_direction, m_azimuth, m_up_vector);
  glm::vec3 elevation_axis = glm::cross(eye_vector, m_up_vector);
  eye_vector = glm::rotate(eye_vector, m_elevation, elevation_axis);
  eye_vector *= m_distance;
  return glm::lookAt(m_center_point + eye_vector, m_center_point, m_up_vector);
}

glm::mat4 ArcballCamera::GetMatrixProjection() const {
  return glm::perspective(glm::radians(FOVY), m_aspect, ZNEAR, ZFAR);
}

void ArcballCamera::SetInitEyeAndUp(const glm::vec3& eye, const glm::vec3& up) {
  glm::vec3 eye_vector = eye - m_center_point;
  glm::vec3 elevation_axis = glm::cross(eye_vector, up);

  m_azimuth = 0.0f;

  // pi/2 - [0, pi] => [pi/2, -pi/2]
  m_elevation = M_PI / 2.0f - std::acos(glm::normalizeDot(eye_vector, up));
  m_elevation = std::clamp(m_elevation, -ELEVATION_ABS_MAX, ELEVATION_ABS_MAX);

  m_distance = glm::length(eye_vector);

  m_eye_direction = glm::normalize(glm::cross(up, elevation_axis));
  m_up_vector = up;
}

}  // namespace cheers
