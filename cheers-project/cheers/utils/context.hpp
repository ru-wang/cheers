#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace cheers {

class Context {
public:
  Context(int window_width, int window_height);
  ~Context() noexcept;

  bool ShouldGlfwWindowExit() const;
  void PollUiEvents() const;
  void SwapBuffer() const;

  void InstallGlfwWindowFocusCallback(GLFWwindowfocusfun callback) const;
  void InstallGlfwWindowSizeCallback(GLFWwindowsizefun callback) const;
  void InstallGlfwFramebufferSizeCallback(GLFWframebuffersizefun callback) const;
  void InstallGlfwKeyCallback(GLFWkeyfun callback) const;
  void InstallGlfwMouseButtonCallback(GLFWmousebuttonfun callback) const;
  void InstallGlfwCursorPosCallback(GLFWcursorposfun callback) const;
  void InstallGlfwScrollCallback(GLFWscrollfun callback) const;

  void UninstallAllGlfwCallbacks() const;

private:
  void CreateGlfwContext(int window_width, int window_height);
  void CreateGlewContext();

  void DestroyGlfwContext();
  void DestroyGlewContext();

  GLFWwindow* m_glfw_window = nullptr;
};

}  // namespace cheers
