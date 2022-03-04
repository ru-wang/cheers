#include "cheers/window/window.hpp"

#include <cmath>

#include <GL/glew.h>

#include "cheers/window/callback.hpp"

namespace cheers {

void Window::CreateContext() {
  m_context = std::make_unique<Context>(m_ui_state.window_size.x, m_ui_state.window_size.y);
  m_context->InstallGlfwWindowFocusCallback(Callback::OnFocusWindow);
  m_context->InstallGlfwWindowSizeCallback(Callback::OnResizeWindow);
  m_context->InstallGlfwFramebufferSizeCallback(Callback::OnResizeFrameBuffer);
  m_context->InstallGlfwKeyCallback(Callback::OnKeyAction);
  m_context->InstallGlfwMouseButtonCallback(Callback::OnMouseButtonAction);
  m_context->InstallGlfwCursorPosCallback(Callback::OnMouseMove);
  m_context->InstallGlfwScrollCallback(Callback::OnMouseScroll);

  m_im_renderer = std::make_unique<ImRenderer>();

  m_scene_layer = std::make_shared<SceneLayer>();
  m_scene_layer->CreateRenderer();
  m_layers.emplace_back(m_scene_layer);
}

void Window::DestroyContext() {
  DeleteRenderPrograms();
  m_im_renderer.reset();
  m_context.reset();
}

void Window::SetInitEyeAndUp(
    float eye_x, float eye_y, float eye_z, float up_x, float up_y, float up_z) {
  m_arcball_camera.SetInitEyeAndUp(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(up_x, up_y, up_z));
  if (std::abs(up_x) > std::abs(up_y) && std::abs(up_x) > std::abs(up_z))
    m_scene_layer->SetGridUpAxis(0);
  else if (std::abs(up_y) > std::abs(up_x) && std::abs(up_y) > std::abs(up_z))
    m_scene_layer->SetGridUpAxis(1);
  else if (std::abs(up_z) > std::abs(up_x) && std::abs(up_z) > std::abs(up_y))
    m_scene_layer->SetGridUpAxis(2);
}

bool Window::WaitForWindowExiting() {
  if (!m_context->ShouldGlfwWindowExit()) {
    CreateRenderPrograms();
    UpdateUiEvents();
    OnUpdateRenderData();
    OnRenderLayer();
    return true;
  } else {
    m_ui_state.ClearStalled();
    return false;
  }
}

void Window::WaitForWindowStalled() {
  m_ui_state.WaitStalled();
}

void Window::CreateRenderPrograms() {
  std::unique_lock lock(m_layer_buffer_mutex, std::try_to_lock);
  if (!lock.owns_lock())
    return;
  for (auto& layer : m_layer_buffer)
    m_layers.emplace_back(std::move(layer))->CreateRenderer();
  m_layer_buffer.clear();
}

void Window::DeleteRenderPrograms() {
  for (const auto& layer : m_layers)
    layer->DestroyRenderer();
}

void Window::UpdateUiEvents() {
  // update mouse action
  m_context->PollUiEvents();
  if (!m_im_renderer->UpdateUiState(m_ui_state))
    m_arcball_camera.UpdateMatrixView(m_ui_state);
  m_arcball_camera.UpdateMatrixProjection(m_ui_state);
  m_matrix_vp = m_arcball_camera.GetMatrixProjection() * m_arcball_camera.GetMatrixView();
  m_ui_state.ClearMouseDeltaState();
}

void Window::OnUpdateRenderData() {
  m_im_renderer->BeginImFrame();
  for (const auto& layer : m_layers)
    layer->UpdateImFrame();
  m_im_renderer->EndImFrame();
  for (const auto& layer : m_layers)
    layer->UpdateRenderData();
}

void Window::OnRenderLayer() {
  glViewport(0, 0, m_ui_state.frame_buffer.x, m_ui_state.frame_buffer.y);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (const auto& layer : m_layers)
    layer->RenderLayer(&m_matrix_vp[0][0]);
  m_im_renderer->RenderImFrame();
  m_context->SwapBuffer();
}

}  // namespace cheers
