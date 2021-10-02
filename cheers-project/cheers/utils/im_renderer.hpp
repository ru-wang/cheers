#pragma once

#include <chrono>

#include "cheers/utils/gl_program_wrapper.hpp"
#include "cheers/utils/window_ui_state.hpp"

namespace cheers {

class ImRenderer {
public:
  ImRenderer();
  ~ImRenderer() noexcept;

  bool UpdateUiState(const WindowUiState& ui_state);
  void BeginImFrame();
  void EndImFrame();
  void RenderImFrame();

private:
  void InitImProgram();
  void InitImContext();
  void BindImFontTexture();

  void SetupImRenderState();
  void InternalRenderImFrame();

  ProgramWrapper m_program;

  std::chrono::steady_clock::time_point m_update_time = std::chrono::steady_clock::now();
};

}  // namespace cheers
