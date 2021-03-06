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
  m_layer_manager = std::make_unique<LayerManager>();
}

void Window::DestroyContext() {
  m_layer_manager.reset();
  m_im_renderer.reset();
  m_context.reset();
}

void Window::SetInitEyeAndUp(
    float eye_x, float eye_y, float eye_z, float up_x, float up_y, float up_z) {
  m_arcball_camera.SetInitEyeAndUp(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(up_x, up_y, up_z));
  if (std::abs(up_x) > std::abs(up_y) && std::abs(up_x) > std::abs(up_z))
    m_layer_manager->GetDefaultLayer()->SetGridUpAxis(0);
  else if (std::abs(up_y) > std::abs(up_x) && std::abs(up_y) > std::abs(up_z))
    m_layer_manager->GetDefaultLayer()->SetGridUpAxis(1);
  else if (std::abs(up_z) > std::abs(up_x) && std::abs(up_z) > std::abs(up_y))
    m_layer_manager->GetDefaultLayer()->SetGridUpAxis(2);
}

bool Window::WaitForWindowExiting() {
  UpdateUiEvents();
  if (!m_context->ShouldGlfwWindowExit()) {
    SwapLayerBuffer();
    UpdateRenderData();
    RenderLayer();
    return true;
  }
  ClearUiStalled();
  return false;
}

void Window::WaitForWindowStalled() { m_ui_state.WaitStalled(); }

void Window::UpdateUiEvents() {
  // update mouse action
  m_context->PollUiEvents();
  if (!m_im_renderer->UpdateUiState(m_ui_state))
    m_arcball_camera.UpdateMatrixView(m_ui_state);
  m_arcball_camera.UpdateMatrixProjection(m_ui_state);
  m_matrix_vp = m_arcball_camera.GetMatrixProjection() * m_arcball_camera.GetMatrixView();
  m_ui_state.ClearMouseDeltaState();
}

void Window::ClearUiStalled() {
  m_ui_state.ClearStalled();
}

void Window::SwapLayerBuffer() {
  std::unique_lock lock(m_layer_buffer_mutex, std::try_to_lock);
  if (!lock.owns_lock())
    return;
  for (auto& layer : m_layer_buffer)
    m_layer_manager->AddSharedLayer(std::move(layer));
  m_layer_buffer.clear();
}

void Window::UpdateRenderData() {
  m_im_renderer->BeginImFrame();
  m_layer_manager->UpdateImFrame();
  m_im_renderer->EndImFrame();
  m_layer_manager->UpdateRenderData();
}

void Window::RenderLayer() {
  glViewport(0, 0, m_ui_state.frame_buffer.x, m_ui_state.frame_buffer.y);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_layer_manager->RenderLayer(&m_matrix_vp[0][0]);
  m_im_renderer->RenderImFrame();
  m_context->SwapBuffer();
}

}  // namespace cheers
