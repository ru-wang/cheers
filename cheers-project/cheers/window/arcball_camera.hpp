#pragma once

#include <glm/glm.hpp>

#include "cheers/utils/window_ui_state.hpp"

namespace cheers {

class ArcballCamera {
public:
  void UpdateMatrixView(const WindowUiState& ui_state);
  void UpdateMatrixProjection(const WindowUiState& ui_state);

  glm::mat4 GetMatrixView() const;
  glm::mat4 GetMatrixProjection() const;

  void set_eye_position(const glm::vec3& vec) noexcept { m_eye_position = vec; }
  void set_center_point(const glm::vec3& vec) noexcept { m_center_point = vec; }
  void set_up_vector(const glm::vec3& vec) noexcept { m_up_vector = vec; }

private:
  static constexpr float ROTATION_RESOLUTION = 1.0 / 3.0f;
  static constexpr float TRANSLATION_RESOLUTION = 1.0 / 200.0f;
  static constexpr float TRANSLATION_Z_RESOLUTION = 1.0 / 2.0f;
  static constexpr float TRANSLATION_Z_MIN = 1.0f;

  static constexpr float ZNEAR = 0.1f;
  static constexpr float ZFAR = 1000.0f;
  static constexpr float FOVY = 60.0f;

  float m_aspect = 1.0f;

  glm::vec3 m_eye_position{-50.0f, 10.0f, 10.0f};
  glm::vec3 m_center_point{0.0f, 0.0f, 0.0f};
  glm::vec3 m_up_vector{0.0f, 0.0f, 1.0f};
};

}  // namespace cheers
