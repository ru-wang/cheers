#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cheers/utils/window_ui_state.hpp"

namespace cheers {

struct Callback {
  static WindowUiState& GetUiState();
  static void OnFocusWindow(GLFWwindow* window, int focused);
  static void OnResizeWindow(GLFWwindow* window, int width, int height);
  static void OnResizeFrameBuffer(GLFWwindow* window, int width, int height);
  static void OnKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void OnMouseButtonAction(GLFWwindow* window, int button, int action, int mods);
  static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
  static void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
};

}  // namespace cheers
