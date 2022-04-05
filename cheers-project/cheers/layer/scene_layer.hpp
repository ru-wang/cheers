#pragma once

#include <array>
#include <string_view>
#include <vector>

#include "cheers/layer/layer.hpp"
#include "cheers/utils/gl_program_wrapper.hpp"

namespace cheers {

class SceneLayer : public Layer {
public:
  static constexpr std::string_view DEFAULT_WINDOW_NAME = "Scene Control Panel";

  ~SceneLayer() noexcept override = default;

  void OnCreateRenderer() override;
  void OnDestroyRenderer() override;

  void OnUpdateImFrame() override;
  void OnUpdateRenderData() override;
  void OnRenderLayer(const float* matrix_vp) override;

  // 0 for X, 1 for Y, 2, Z, otherwise do nothing
  void SetGridUpAxis(int up_axis);

private:
  struct RenderData {
    std::array<float, 3> background_color{0.20f, 0.25f, 0.30f};
    std::array<float, 3> grid_color{0.80f, 0.75f, 0.70f};
    int grid_num_per_quad = 10;
    float grid_metric_size = 1.0f;
    int grid_up_axis = 2;

    // generated data
    std::vector<int> grid_data;
  };

  void InitAxisProgram();
  void InitGridProgram();

  void RenderAxis(const float* matrix_vp) const;
  void RenderGrid(const float* matrix_vp) const;

  RenderData m_render_data;
};

}  // namespace cheers
