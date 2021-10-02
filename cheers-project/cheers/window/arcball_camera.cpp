#include "cheers/window/arcball_camera.hpp"

#include <cmath>

#include <glm/ext.hpp>

namespace cheers {

namespace {

template <typename T>
constexpr float Radians(T angle) {
  return angle / 180.0 * M_PI;
}

}  // namespace

void ArcballCamera::UpdateMatrixView(const WindowUiState& ui_state) {
  const auto& mouse_action = ui_state.mouse_action;
  glm::vec3 view_dir = m_center_point - m_eye_position;

  if (mouse_action.button_pressed.at(GLFW_MOUSE_BUTTON_LEFT)) {
    glm::vec3 eye_vector = -view_dir;

    glm::vec3 azimuth_axis = glm::normalize(m_up_vector);
    glm::vec3 pitch_axis = glm::normalize(glm::cross(m_up_vector, view_dir));
    float azimuth_radians = Radians(-mouse_action.pos_delta.x) * ROTATION_RESOLUTION;
    float pitch_radians = Radians(mouse_action.pos_delta.y) * ROTATION_RESOLUTION;

    eye_vector = glm::rotate(eye_vector, azimuth_radians, azimuth_axis);
    eye_vector = glm::rotate(eye_vector, pitch_radians, pitch_axis);
    m_eye_position = m_center_point + eye_vector;
    m_up_vector = glm::rotate(m_up_vector, pitch_radians, pitch_axis);
  } else if (mouse_action.button_pressed.at(GLFW_MOUSE_BUTTON_RIGHT)) {
    glm::vec3 hori_axis = glm::normalize(glm::cross(m_up_vector, view_dir));
    glm::vec3 vert_axis = glm::normalize(glm::cross(view_dir, hori_axis));
    glm::vec3 translation =
        mouse_action.pos_delta.x * hori_axis + mouse_action.pos_delta.y * vert_axis;
    m_eye_position += glm::length(view_dir) * translation * TRANSLATION_RESOLUTION;
    m_center_point += glm::length(view_dir) * translation * TRANSLATION_RESOLUTION;
  } else {
    float translation = mouse_action.wheel_delta.y * TRANSLATION_Z_RESOLUTION;
    if (translation > glm::length(view_dir) - TRANSLATION_Z_MIN)
      m_eye_position = m_center_point - glm::normalize(view_dir) * TRANSLATION_Z_MIN;
    else
      m_eye_position += translation * glm::normalize(view_dir);
  }
}

void ArcballCamera::UpdateMatrixProjection(const WindowUiState& ui_state) {
  m_aspect = ui_state.frame_buffer.x * 1.0f / ui_state.frame_buffer.y;
}

glm::mat4 ArcballCamera::GetMatrixView() const {
  return glm::lookAt(m_eye_position, m_center_point, m_up_vector);
}

glm::mat4 ArcballCamera::GetMatrixProjection() const {
  return glm::perspective(Radians(FOVY), m_aspect, ZNEAR, ZFAR);
}

}  // namespace cheers
