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

  void SetInitEyeAndUp(const glm::vec3& eye, const glm::vec3& up);

private:
  static constexpr float ROTATION_RESOLUTION = 0.75f;
  static constexpr float TRANSLATION_RESOLUTION = 0.005f;
  static constexpr float TRANSLATION_Z_RESOLUTION = 0.025f;
  static constexpr float TRANSLATION_Z_MIN = 1.0f;

  static constexpr float ELEVATION_ABS_MAX = glm::radians(89.9f);

  static constexpr float ZNEAR = 0.1f;
  static constexpr float ZFAR = 1000.0f;
  static constexpr float FOVY = 60.0f;

  float m_aspect = 1.0f;

  float m_azimuth = 0.0f;
  float m_elevation = glm::radians(45.0f);
  float m_distance = 50.0f;

  glm::vec3 m_eye_direction{-1.0f, 0.0f, 0.0f};
  glm::vec3 m_up_vector{0.0f, 0.0f, 1.0f};
  glm::vec3 m_center_point{0.0f, 0.0f, 0.0f};
};

}  // namespace cheers
