#pragma once

#include <deque>
#include <mutex>

#include "cheers/utils/gl_program_wrapper.hpp"

namespace cheers {

class Layer {
public:
  virtual ~Layer() noexcept = 0;

  virtual void OnCreateRenderer() {}
  virtual void OnDestroyRenderer() {}

  virtual void OnUpdateImFrame() {}
  virtual void OnUpdateRenderData() {}
  virtual void OnRenderLayer(const float*) {}

protected:
  void add_render_program() { m_render_programs.emplace_back(); }
  void clear_render_program() noexcept { m_render_programs.clear(); }

  size_t num_render_programs() const noexcept { return m_render_programs.size(); }
  auto& render_program(size_t index) const noexcept { return m_render_programs.at(index); }
  auto& render_program(size_t index) noexcept { return m_render_programs.at(index); }

  auto& render_data_mutex() const noexcept { return m_render_data_mutex; }
  auto& render_data_mutex() noexcept { return m_render_data_mutex; }

private:
  std::deque<ProgramWrapper> m_render_programs;
  std::mutex m_render_data_mutex;
};

inline Layer::~Layer() noexcept = default;

}  // namespace cheers
