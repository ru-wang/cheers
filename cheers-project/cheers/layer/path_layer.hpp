#pragma once

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "cheers/layer/layer.hpp"
#include "cheers/proto/path.pb.h"
#include "cheers/utils/gl_program_wrapper.hpp"

namespace cheers {

class PathLayer : public Layer {
public:
  ~PathLayer() noexcept override = default;

  void OnCreateRenderer() override;
  void OnDestroyRenderer() override;

  void OnUpdateRenderData() override;
  void OnRenderLayer(const float* matrix_vp) override;

  int CreatePath(const message::PathConfig& config) {
    std::lock_guard lock(render_data_mutex());
    if (!m_render_data.name_index.count(config.name())) {
      m_render_data.name_index.emplace(config.name(), m_render_data.path_messages.size());
      m_render_data.path_messages.emplace_back().mutable_config()->CopyFrom(config);
      if (m_render_data.path_messages.back().config().color().a() == 0.0f)
        m_render_data.path_messages.back().mutable_config()->mutable_color()->set_a(1.0f);
      m_render_data.dirty = true;
    }
    return m_render_data.name_index.at(config.name());
  }

  void ClearPath(const std::string& name) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.path_messages.at(index).clear_pose();
    m_render_data.dirty = true;
  }

  void ClearPath(int index) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.path_messages.at(index).clear_pose();
    m_render_data.dirty = true;
  }

  void AddPose(const std::string& name, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.path_messages.at(index).add_pose()->CopyFrom(pose);
    m_render_data.dirty = true;
  }

  void AddPose(int index, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.path_messages.at(index).add_pose()->CopyFrom(pose);
    m_render_data.dirty = true;
  }

  void UpdateHeadPose(int index, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    auto& message = m_render_data.path_messages.at(index);
    message.mutable_pose(0)->CopyFrom(pose);
    m_render_data.dirty = true;
  }

  void UpdateHeadPose(const std::string& name, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    auto& message = m_render_data.path_messages.at(index);
    message.mutable_pose(0)->CopyFrom(pose);
    m_render_data.dirty = true;
  }

  void UpdateTailPose(int index, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    auto& message = m_render_data.path_messages.at(index);
    message.mutable_pose(message.pose_size() - 1)->CopyFrom(pose);
    m_render_data.dirty = true;
  }

  void UpdateTailPose(const std::string& name, const message::Pose& pose) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    auto& message = m_render_data.path_messages.at(index);
    message.mutable_pose(message.pose_size() - 1)->CopyFrom(pose);
    m_render_data.dirty = true;
  }

private:
  struct RenderData {
    struct PathData {
      float line_width = 2.0f;
      std::array<float, 4> color;
      std::array<float, 16> sensor_pose;

      std::vector<float> vertex_data;
      std::vector<float> pose_data;
    };

    bool dirty = false;
    std::unordered_map<std::string, int> name_index;
    std::vector<message::Path> path_messages;

    // generated data
    std::vector<PathData> path_data;
  };

  enum {
    VBO_PATH_DATA,
    VBO_NODE_DATA,
    VBO_COUNT,
  };

  void AugmentVaoData();
  void UpdatePathData();

  RenderData m_render_data;
};

}  // namespace cheers
