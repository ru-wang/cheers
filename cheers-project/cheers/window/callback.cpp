#include "cheers/window/callback.hpp"

#include "cheers/window/window.hpp"

namespace cheers {

WindowUiState& Callback::GetUiState() {
  return Window::Instance().m_ui_state;
}

void Callback::OnFocusWindow(GLFWwindow*, int focused) {
  if (focused)
    GetUiState().window_focused_regained = focused;
}

void Callback::OnResizeWindow(GLFWwindow*, int width, int height) {
  GetUiState().window_size.x = width;
  GetUiState().window_size.y = height;
}

void Callback::OnResizeFrameBuffer(GLFWwindow*, int width, int height) {
  GetUiState().frame_buffer.x = width;
  GetUiState().frame_buffer.y = height;
}

void Callback::OnKeyAction(GLFWwindow* window, int key, int, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_Q:
        if (mods == GLFW_MOD_CONTROL)
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_SPACE:
        GetUiState().ToggleStalled();
        break;
      default:
        break;
    }
  }
}

void Callback::OnMouseButtonAction(GLFWwindow*, int button, int action, int) {
  auto& mouse_action = GetUiState().mouse_action;
  if (button >= 0 && button < static_cast<int>(mouse_action.button_pressed.size())) {
    if (action == GLFW_PRESS)
      mouse_action.button_pressed.at(button) = 1;
    else if (action == GLFW_RELEASE)
      mouse_action.button_pressed.at(button) = 0;
  }
}

void Callback::OnMouseMove(GLFWwindow*, double xpos, double ypos) {
  auto& mouse_action = GetUiState().mouse_action;

  if (GetUiState().window_focused_regained) {
    mouse_action.pos.x = xpos;
    mouse_action.pos.y = ypos;
    GetUiState().window_focused_regained = false;
  } else {
    mouse_action.pos_delta.x = xpos - mouse_action.pos.x;
    mouse_action.pos_delta.y = ypos - mouse_action.pos.y;
    mouse_action.pos.x = xpos;
    mouse_action.pos.y = ypos;
  }
}

void Callback::OnMouseScroll(GLFWwindow*, double xoffset, double yoffset) {
  auto& mouse_action = GetUiState().mouse_action;
  mouse_action.wheel_delta.x = xoffset;
  mouse_action.wheel_delta.y = yoffset;
}

}  // namespace cheers
