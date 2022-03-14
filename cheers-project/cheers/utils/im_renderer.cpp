#include "cheers/utils/im_renderer.hpp"

#include <filesystem>
#include <utility>

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <implot.h>

#include "cheers/resource/font_absolute_path.hpp"
#include "cheers/resource/shader_absolute_path.hpp"
#include "cheers/utils/gl_backup_state.hpp"
#include "cheers/utils/throw_and_check.hpp"

namespace cheers {

ImRenderer::ImRenderer() {
  InitImProgram();
  InitImContext();
  BindImFontTexture();
}

ImRenderer::~ImRenderer() noexcept {
  ImGui::GetIO().Fonts->SetTexID(nullptr);
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

bool ImRenderer::UpdateUiState(const WindowUiState& ui_state) {
  auto& io = ImGui::GetIO();

  // step 1: update window size and framebuffer size
  float w = ui_state.frame_buffer.x;
  float h = ui_state.frame_buffer.y;
  float display_w = ui_state.window_size.x;
  float display_h = ui_state.window_size.y;
  io.DisplaySize = ImVec2(w, h);
  if (w > 0.0f && h > 0.0f)
    io.DisplayFramebufferScale = ImVec2(display_w / w, display_h / h);

  // step 2: update frame rate
  auto update_time = std::chrono::steady_clock::now();
  io.DeltaTime = std::chrono::duration<float>(update_time - m_update_time).count();
  m_update_time = update_time;

  // step 3: update cursor position and wheel position
  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
    io.MouseDown[i] = ui_state.mouse_action.button_pressed.at(i);
  io.MousePos = ImVec2(ui_state.mouse_action.pos.x, ui_state.mouse_action.pos.y);
  io.MouseWheelH += ui_state.mouse_action.wheel_delta.x;
  io.MouseWheel += ui_state.mouse_action.wheel_delta.y;

  // step 4: return true if ImGui windows focused
  return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void ImRenderer::BeginImFrame() {
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImRenderer::EndImFrame() {
  ImGui::EndFrame();
  ImGui::Render();
}

void ImRenderer::RenderImFrame() {
  const auto* drawdata = ImGui::GetDrawData();
  // avoid rendering when minimized, scale coordinates for retina displays
  // (screen coordinates != framebuffer coordinates)
  int fb_width = (int)(drawdata->DisplaySize.x * drawdata->FramebufferScale.x);
  int fb_height = (int)(drawdata->DisplaySize.y * drawdata->FramebufferScale.y);
  if (fb_width <= 0 || fb_height <= 0)
    return;

  GlBackupState render_state_backup;
  SetupImRenderState(drawdata, fb_width, fb_height);
  InternalRenderImFrame(drawdata, fb_width, fb_height);
}

void ImRenderer::InitImProgram() {
  ShaderObject vshader(imgui_vshader_absolute_path);
  ShaderObject fshader(imgui_fshader_absolute_path);
  m_program.Attach(vshader, fshader);
  m_program.Link();
  m_program.Detach(vshader, fshader);

  m_program.GenVao(1);
  m_program.Vao(0).GenVbo(1);
  m_program.Vao(0).GenEbo(1);
}

void ImRenderer::InitImContext() {
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGui::StyleColorsDark();

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::GetIO().BackendPlatformName = "ImGui binding for GLFW3";
  ImGui::GetIO().BackendRendererName = "ImGui binding for OpenGL3";
  ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
}

void ImRenderer::BindImFontTexture() {
  m_program.GenText(1);
  m_program.Text(0).Bind();

  unsigned char* font_text;
  int width, height;
  ImGui::GetIO().Fonts->AddFontFromFileTTF(karla_ttf_absolute_path.c_str(), 14);
  ImGui::GetIO().Fonts->AddFontFromFileTTF(karla_ttf_absolute_path.c_str(), 18);
  ImGui::GetIO().Fonts->AddFontFromFileTTF(karla_ttf_absolute_path.c_str(), 22);

  ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_text, &width, &height);
  ImGui::GetIO().Fonts->SetTexID(
      reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_program.Text(0).name())));
  CHECK(ImGui::GetIO().Fonts->IsBuilt(), "{:10}:{} | {}", __FILE__, __LINE__, "font not built");

  ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[1];

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_text);
}

void ImRenderer::SetupImRenderState(const void* data, int fb_width, int fb_height) {
  auto drawdata = static_cast<const ImDrawData*>(data);

  // setup render state:
  //   - alpha-blending enabled
  //   - no face culling
  //   - no depth testing
  //   - scissor enabled,
  //   - polygon fill
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_SCISSOR_TEST);
  glDisable(GL_PRIMITIVE_RESTART);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glViewport(0, 0, fb_width, fb_height);
  float l = drawdata->DisplayPos.x;
  float r = drawdata->DisplayPos.x + drawdata->DisplaySize.x;
  float t = drawdata->DisplayPos.y;
  float b = drawdata->DisplayPos.y + drawdata->DisplaySize.y;

  GLint current_clip_origin = 0;
  glGetIntegerv(GL_CLIP_ORIGIN, &current_clip_origin);

  // swap top and bottom if origin is upper left
  if (current_clip_origin == GL_UPPER_LEFT)
    std::swap(t, b);
  glm::mat4 ortho = glm::ortho(l, r, b, t, 0.0f, 1.0f);

  m_program.Use();
  glUniform1i(m_program.Uniform("Texture"), 0);
  glUniformMatrix4fv(m_program.Uniform("ProjMtx"), 1, GL_FALSE, &ortho[0][0]);
  glBindSampler(0, 0);

  m_program.Vao(0).Bind();
  m_program.EnableAttrib("Position", "UV", "Color");

  m_program.Vao(0).Vbo(0).Bind();
  m_program.Vao(0).Ebo(0).Bind();
  glVertexAttribPointer(m_program.Attrib("Position"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                        reinterpret_cast<void*>(IM_OFFSETOF(ImDrawVert, pos)));
  glVertexAttribPointer(m_program.Attrib("UV"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                        reinterpret_cast<void*>(IM_OFFSETOF(ImDrawVert, uv)));
  glVertexAttribPointer(m_program.Attrib("Color"), 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
                        reinterpret_cast<void*>(IM_OFFSETOF(ImDrawVert, col)));
}

void ImRenderer::InternalRenderImFrame(const void* data, int fb_width, int fb_height) {
  auto drawdata = static_cast<const ImDrawData*>(data);

  // will project scissor/clipping rectangles into framebuffer space
  ImVec2 clip_off = drawdata->DisplayPos;  // (0,0) unless using multi-viewports
  ImVec2 clip_scale =
      drawdata->FramebufferScale;  // (1,1) unless using retina display which are often (2,2)

  for (int i = 0; i < drawdata->CmdListsCount; ++i) {
    const auto* cmd_list = drawdata->CmdLists[i];

    auto vtx_buffer_size = static_cast<GLsizeiptr>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
    auto idx_buffer_size = static_cast<GLsizeiptr>(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    auto vtx_buffer_data = static_cast<const void*>(cmd_list->VtxBuffer.Data);
    auto idx_buffer_data = static_cast<const void*>(cmd_list->IdxBuffer.Data);
    glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, vtx_buffer_data, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, idx_buffer_data, GL_STREAM_DRAW);

    for (const auto& pcmd : cmd_list->CmdBuffer) {
      if (pcmd.UserCallback) {
        if (pcmd.UserCallback == ImDrawCallback_ResetRenderState)
          SetupImRenderState(data, fb_width, fb_height);
        else
          pcmd.UserCallback(cmd_list, &pcmd);
      } else {
        // project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_min((pcmd.ClipRect.x - clip_off.x) * clip_scale.x,
                        (pcmd.ClipRect.y - clip_off.y) * clip_scale.y);
        ImVec2 clip_max((pcmd.ClipRect.z - clip_off.x) * clip_scale.x,
                        (pcmd.ClipRect.w - clip_off.y) * clip_scale.y);
        if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
          continue;

        // apply scissor/clipping rectangle (y is inverted in opengl)
        glScissor(static_cast<GLint>(clip_min.x),
                  static_cast<GLint>(fb_height - clip_max.y),
                  static_cast<GLsizei>(clip_max.x - clip_min.x),
                  static_cast<GLsizei>(clip_max.y - clip_min.y));

        // bind texture, draw
        glBindTexture(GL_TEXTURE_2D, reinterpret_cast<intptr_t>(pcmd.GetTexID()));
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 static_cast<GLsizei>(pcmd.ElemCount),
                                 sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                 reinterpret_cast<void*>(pcmd.IdxOffset * sizeof(ImDrawIdx)),
                                 static_cast<GLint>(pcmd.VtxOffset));
      }
    }
  }
}

}  // namespace cheers
