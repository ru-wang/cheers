#pragma once

#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "cheers/layer/layer.hpp"
#include "cheers/utils/context.hpp"
#include "cheers/utils/im_renderer.hpp"
#include "cheers/utils/window_ui_state.hpp"
#include "cheers/window/arcball_camera.hpp"

namespace cheers {

class Window {
public:
  static Window& Instance() noexcept {
    static Window instance;
    return instance;
  }

  template <typename T, typename... Args>
  std::shared_ptr<T> InstallSharedLayer(Args&&... args) {
    auto layer = std::make_shared<T>(std::forward<Args>(args)...);
    std::lock_guard lock(m_layer_buffer_mutex);
    m_layer_buffer.push_back(std::static_pointer_cast<Layer>(layer));
    return layer;
  }

  void CreateContext();
  void DestroyContext();

  void SetInitEyePosition(float x, float y, float z);
  void SetInitUpVector(float x, float y, float z);

  bool WaitForWindowExiting();
  void WaitForWindowStalled();

  auto& ui_state() const noexcept { return m_ui_state; }
  auto& ui_state() noexcept { return m_ui_state; }

private:
  void CreateRenderPrograms();
  void DeleteRenderPrograms();

  void UpdateUiEvents();
  void OnUpdateRenderData();
  void OnRenderLayer();

  WindowUiState m_ui_state;
  ArcballCamera m_arcball_camera;
  glm::mat4 m_matrix_vp{1.0f};

  std::unique_ptr<Context> m_context;
  std::unique_ptr<ImRenderer> m_im_renderer;

  std::mutex m_layer_buffer_mutex;
  std::vector<std::shared_ptr<Layer>> m_layer_buffer;
  std::vector<std::shared_ptr<Layer>> m_layers;
};

}  // namespace cheers