#pragma once

#include <condition_variable>
#include <array>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace cheers {

struct MouseAction {
  glm::vec2 pos{0.0f};
  glm::vec2 pos_delta{0.0f};
  glm::vec2 wheel_delta{0.0f};
  std::array<int, GLFW_MOUSE_BUTTON_LAST> button_pressed{0};
};

struct WindowUiState {
  bool window_focused_regained = false;
  glm::ivec2 window_size{1280, 720};
  glm::ivec2 frame_buffer{1280, 720};
  MouseAction mouse_action;

  std::mutex stalled_cv_mutex;
  std::condition_variable stalled_cv;
  bool stalled = false;

  void ClearMouseDeltaState() {
    mouse_action.pos_delta.x = 0.0f;
    mouse_action.pos_delta.y = 0.0f;
    mouse_action.wheel_delta.x = 0.0f;
    mouse_action.wheel_delta.y = 0.0f;
  }

  void ToggleStalled() {
    std::unique_lock lock(stalled_cv_mutex);
    stalled ^= 1;
    stalled_cv.notify_all();
  }

  void ClearStalled() {
    std::unique_lock lock(stalled_cv_mutex);
    stalled = false;
    stalled_cv.notify_all();
  }

  void WaitStalled() {
    std::unique_lock lock(stalled_cv_mutex);
    stalled_cv.wait(lock, [this]() { return !stalled; });
  }
};

}  // namespace cheers
