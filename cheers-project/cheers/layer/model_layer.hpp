#pragma once

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "cheers/layer/layer.hpp"
#include "cheers/utils/gl_program_wrapper.hpp"
#include "cheers/proto/model.pb.h"

namespace cheers {

class ModelLayer : public Layer {
public:
  ~ModelLayer() noexcept override = default;

  int CreateModel(const message::ModelConfig& config) {
    std::lock_guard lock(render_data_mutex());
    if (!m_render_data.name_index.count(config.name())) {
      m_render_data.name_index.emplace(config.name(), m_render_data.model_messages.size());
      m_render_data.model_messages.emplace_back().mutable_config()->CopyFrom(config);
      m_render_data.dirty = true;
    }
    return m_render_data.name_index.at(config.name());
  }

  void ClearModel(const std::string& name) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.model_messages.at(index).clear_vertex();
    m_render_data.model_messages.at(index).clear_triangle();
    m_render_data.dirty = true;
  }

  void ClearModel(int index) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.model_messages.at(index).clear_vertex();
    m_render_data.model_messages.at(index).clear_triangle();
    m_render_data.dirty = true;
  }

  void AddVertex(const std::string& name, const message::Vector3f& vertex) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.model_messages.at(index).add_vertex()->CopyFrom(vertex);
    m_render_data.dirty = true;
  }

  void AddVertex(int index, const message::Vector3f& vertex) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.model_messages.at(index).add_vertex()->CopyFrom(vertex);
    m_render_data.dirty = true;
  }

  template <typename InputIt>
  void AddVertices(const std::string& name, InputIt first, InputIt last) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.model_messages.at(index).mutable_vertex()->Add(first, last);
    m_render_data.dirty = true;
  }

  template <typename InputIt>
  void AddVertices(int index, InputIt first, InputIt last) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.model_messages.at(index).mutable_vertex()->Add(first, last);
    m_render_data.dirty = true;
  }

  void AddTriangle(const std::string& name, const message::Vector3i& triangle) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.model_messages.at(index).add_triangle()->mutable_index()->CopyFrom(triangle);
    m_render_data.dirty = true;
  }

  void AddTriangle(int index, const message::Vector3i& triangle) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.model_messages.at(index).add_triangle()->mutable_index()->CopyFrom(triangle);
    m_render_data.dirty = true;
  }

private:
  struct RenderData {
    struct ModelData {
      float point_size = 2.0f;
      std::array<float, 4> color;

      std::vector<float> vertex_data;
      std::vector<short> index_data;
      std::vector<float> normal_data;
    };

    bool dirty = false;
    std::unordered_map<std::string, int> name_index;
    std::vector<message::Model> model_messages;

    // generated
    std::vector<ModelData> model_data;
  };

  void OnCreateRenderer() override;
  void OnDestroyRenderer() override;

  void OnUpdateRenderData() override;
  void OnRenderLayer(const float* matrix_vp) override;

  void AugmentVaoData();
  void UpdateModelData();

  RenderData m_render_data;
};

}  // namespace cheers
