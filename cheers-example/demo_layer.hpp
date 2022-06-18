#pragma once

#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "cheers/layer/layer.hpp"

class DemoLayer : public cheers::Layer {
public:
  explicit DemoLayer(size_t max_num_object);
  ~DemoLayer() noexcept override = default;

  void OnUpdateRenderData() override;
  void OnRenderLayer(const float* matrix_vp) override;

private:
  struct RenderData {
    struct Object {
      std::vector<glm::vec3> vertex;
      std::vector<glm::vec3> color;
      std::vector<glm::mat4> pose;
    };

    static constexpr size_t NUM_VERTICES_PER_OBJECT = 10000;
    static constexpr float POINT_SIZE = 5.0f;
    static constexpr float ROTATION_RADIUS = 10.0f;

    glm::mat4 extrinsics;
    std::vector<Object> local_objects;

    // generated
    std::vector<std::pair<size_t, std::vector<float>>> bufferdata;
  };

  RenderData m_render_data;
};
