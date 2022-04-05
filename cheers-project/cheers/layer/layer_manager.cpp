#include "cheers/layer/layer_manager.hpp"

#include <utility>

namespace cheers {

LayerManager::LayerManager() {
  m_default_layer = std::make_shared<SceneLayer>();
  m_default_layer->OnCreateRenderer();
  m_layers.push_back(std::static_pointer_cast<Layer>(m_default_layer));
}

LayerManager::~LayerManager() noexcept {
  for (const auto& layer : m_layers)
    layer->OnDestroyRenderer();
}

std::shared_ptr<SceneLayer> LayerManager::GetDefaultLayer() const {
  return m_default_layer;
}

void LayerManager::AddSharedLayer(std::shared_ptr<Layer> layer) {
  m_layers.emplace_back(std::move(layer))->OnCreateRenderer();
}

void LayerManager::UpdateImFrame() {
  for (const auto& layer : m_layers)
    layer->OnUpdateImFrame();
}

void LayerManager::UpdateRenderData() {
  for (const auto& layer : m_layers)
    layer->OnUpdateRenderData();
}

void LayerManager::RenderLayer(const float* matrix_vp) {
  for (const auto& layer : m_layers)
    layer->OnRenderLayer(matrix_vp);
}

}  // namespace cheers
