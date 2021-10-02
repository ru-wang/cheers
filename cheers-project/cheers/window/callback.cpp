#include "cheers/window/callback.hpp"

#include "cheers/window/window.hpp"

namespace cheers {

void Callback::OnFocusWindow(GLFWwindow*, int focused) {
  if (focused)
    Window::Instance().ui_state().window_focused_regained = focused;
}

void Callback::OnResizeWindow(GLFWwindow*, int width, int height) {
  Window::Instance().ui_state().window_size.x = width;
  Window::Instance().ui_state().window_size.y = height;
}

void Callback::OnResizeFrameBuffer(GLFWwindow*, int width, int height) {
  Window::Instance().ui_state().frame_buffer.x = width;
  Window::Instance().ui_state().frame_buffer.y = height;
}

void Callback::OnKeyAction(GLFWwindow* window, int key, int, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_Q:
        if (mods == GLFW_MOD_CONTROL)
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_SPACE:
        Window::Instance().ui_state().ToggleStalled();
        break;
      default:
        break;
    }
  }
}

void Callback::OnMouseButtonAction(GLFWwindow*, int button, int action, int) {
  auto& mouse_action = Window::Instance().ui_state().mouse_action;
  if (button >= 0 && button < static_cast<int>(mouse_action.button_pressed.size())) {
    if (action == GLFW_PRESS)
      mouse_action.button_pressed.at(button) = 1;
    else if (action == GLFW_RELEASE)
      mouse_action.button_pressed.at(button) = 0;
  }
}

void Callback::OnMouseMove(GLFWwindow*, double xpos, double ypos) {
  auto& mouse_action = Window::Instance().ui_state().mouse_action;

  if (Window::Instance().ui_state().window_focused_regained) {
    mouse_action.pos.x = xpos;
    mouse_action.pos.y = ypos;
    Window::Instance().ui_state().window_focused_regained = false;
  } else {
    mouse_action.pos_delta.x = xpos - mouse_action.pos.x;
    mouse_action.pos_delta.y = ypos - mouse_action.pos.y;
    mouse_action.pos.x = xpos;
    mouse_action.pos.y = ypos;
  }
}

void Callback::OnMouseScroll(GLFWwindow*, double xoffset, double yoffset) {
  auto& mouse_action = Window::Instance().ui_state().mouse_action;
  mouse_action.wheel_delta.x = xoffset;
  mouse_action.wheel_delta.y = yoffset;
}

}  // namespace cheers
