#include "cheers/utils/context.hpp"

#include "cheers/utils/throw_and_check.hpp"

namespace cheers {

namespace {

void GlfwErrorCallback(int error, const char* desc) { THROW("{} - {}", error, desc); }

}  // namespace

Context::Context(int window_width, int window_height) {
  CreateGlfwContext(window_width, window_height);
  CreateGlewContext();
}

Context::~Context() noexcept {
  DestroyGlewContext();
  DestroyGlfwContext();
}

bool Context::ShouldGlfwWindowExit() const {
  return glfwWindowShouldClose(m_glfw_window);
}

void Context::PollUiEvents() const {
  glfwPollEvents();
}

void Context::SwapBuffer() const {
  glfwSwapBuffers(m_glfw_window);
}

void Context::InstallGlfwWindowFocusCallback(GLFWwindowfocusfun callback) const {
  glfwSetWindowFocusCallback(m_glfw_window, callback);
}

void Context::InstallGlfwWindowSizeCallback(GLFWwindowsizefun callback) const {
  glfwSetWindowSizeCallback(m_glfw_window, callback);
}

void Context::InstallGlfwFramebufferSizeCallback(GLFWframebuffersizefun callback) const {
  glfwSetFramebufferSizeCallback(m_glfw_window, callback);
}

void Context::InstallGlfwKeyCallback(GLFWkeyfun callback) const {
  glfwSetKeyCallback(m_glfw_window, callback);
}

void Context::InstallGlfwMouseButtonCallback(GLFWmousebuttonfun callback) const {
  glfwSetMouseButtonCallback(m_glfw_window, callback);
}

void Context::InstallGlfwCursorPosCallback(GLFWcursorposfun callback) const {
  glfwSetCursorPosCallback(m_glfw_window, callback);
}

void Context::InstallGlfwScrollCallback(GLFWscrollfun callback) const {
  glfwSetScrollCallback(m_glfw_window, callback);
}

void Context::UninstallAllGlfwCallbacks() const {
  glfwSetWindowSizeCallback(m_glfw_window, nullptr);
  glfwSetFramebufferSizeCallback(m_glfw_window, nullptr);
  glfwSetKeyCallback(m_glfw_window, nullptr);
  glfwSetMouseButtonCallback(m_glfw_window, nullptr);
  glfwSetCursorPosCallback(m_glfw_window, nullptr);
  glfwSetScrollCallback(m_glfw_window, nullptr);
}

void Context::CreateGlfwContext(int window_width, int window_height) {
  glfwSetErrorCallback(GlfwErrorCallback);
  CHECK(glfwInit(), "{:10}:{} | {}", __FILE__, __LINE__, "failed to init GLFW3");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_glfw_window = glfwCreateWindow(window_width, window_height, "cheers", nullptr, nullptr);
  CHECK(m_glfw_window, "{:10}:{} | {}", __FILE__, __LINE__, "failed to create m_glfw_window");
  glfwMakeContextCurrent(m_glfw_window);
}

void Context::CreateGlewContext() {
  GLenum error = glewInit();
  CHECK(error == GLEW_OK, "{:10}:{} | {}", __FILE__, __LINE__, glewGetErrorString(error));
}

void Context::DestroyGlfwContext() {
  glfwDestroyWindow(m_glfw_window);
  glfwTerminate();
}

void Context::DestroyGlewContext() {}

}  // namespace cheers
