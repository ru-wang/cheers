#pragma once

#include <memory>
#include <vector>

#include "cheers/layer/layer.hpp"
#include "cheers/layer/scene_layer.hpp"

namespace cheers {

class LayerManager {
public:
  LayerManager();
  ~LayerManager() noexcept;

  std::shared_ptr<SceneLayer> GetDefaultLayer() const;
  void AddSharedLayer(std::shared_ptr<Layer> layer);

  void UpdateImFrame();
  void UpdateRenderData();
  void RenderLayer(const float* matrix_vp);

private:
  std::shared_ptr<SceneLayer> m_default_layer;
  std::vector<std::shared_ptr<Layer>> m_layers;
};

}  // namespace cheers
