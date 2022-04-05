#pragma once

#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "cheers/layer/layer.hpp"
#include "cheers/layer/layer_manager.hpp"
#include "cheers/utils/context.hpp"
#include "cheers/utils/im_renderer.hpp"
#include "cheers/utils/window_ui_state.hpp"
#include "cheers/window/arcball_camera.hpp"
#include "cheers/window/callback.hpp"

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

  void SetInitEyeAndUp(float eye_x, float eye_y, float eye_z, float up_x, float up_y, float up_z);

  bool WaitForWindowExiting();
  void WaitForWindowStalled();

private:
  void UpdateUiEvents();
  void ClearUiStalled();

  void SwapLayerBuffer();
  void UpdateRenderData();
  void RenderLayer();

  WindowUiState m_ui_state;
  ArcballCamera m_arcball_camera;
  glm::mat4 m_matrix_vp{1.0f};

  std::unique_ptr<Context> m_context;
  std::unique_ptr<ImRenderer> m_im_renderer;
  std::unique_ptr<LayerManager> m_layer_manager;

  std::mutex m_layer_buffer_mutex;
  std::vector<std::shared_ptr<Layer>> m_layer_buffer;

  friend WindowUiState& ::cheers::Callback::GetUiState();
};

}  // namespace cheers
