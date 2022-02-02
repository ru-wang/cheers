#include "cheers/utils/im_renderer.hpp"

#include <filesystem>

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <imgui.h>

#include "cheers/shader/shader_absolute_path.hpp"
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
  auto docker_flags =
      ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), docker_flags);
}

void ImRenderer::EndImFrame() {
  ImGui::EndFrame();
  ImGui::Render();
}

void ImRenderer::RenderImFrame() {
  GlBackupState render_state_backup;
  SetupImRenderState();
  InternalRenderImFrame();
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
  ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_text, &width, &height);
  ImGui::GetIO().Fonts->SetTexID(
      reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_program.Text(0).name())));
  CHECK(ImGui::GetIO().Fonts->IsBuilt(), "{:10}:{} | {}", __FILE__, __LINE__, "font not built");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_text);
}

void ImRenderer::SetupImRenderState() {
  const auto* drawdata = ImGui::GetDrawData();

  // avoid rendering when minimized, scale coordinates for retina displays
  // (screen coordinates != framebuffer coordinates)
  float fb_width = drawdata->DisplaySize.x * drawdata->FramebufferScale.x;
  float fb_height = drawdata->DisplaySize.y * drawdata->FramebufferScale.y;
  if (fb_width <= 0.0f && fb_height <= 0.0f)
    return;

  glActiveTexture(GL_TEXTURE0);

  // setup render state:
  //   - alpha-blending enabled
  //   - no face culling
  //   - no depth testing
  //   - scissor enabled,
  //   - polygon fill
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glDisable(GL_PRIMITIVE_RESTART);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glViewport(0, 0, static_cast<GLsizei>(fb_width), static_cast<GLsizei>(fb_height));
  glm::mat4 ortho = glm::ortho(
      drawdata->DisplayPos.x,
      drawdata->DisplayPos.x + drawdata->DisplaySize.x,
      drawdata->DisplayPos.y + drawdata->DisplaySize.y,
      drawdata->DisplayPos.y,
      0.0f, 1.0f);

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

void ImRenderer::InternalRenderImFrame() {
  const auto* drawdata = ImGui::GetDrawData();

  // avoid rendering when minimized, scale coordinates for retina displays
  // (screen coordinates != framebuffer coordinates)
  float fb_width = drawdata->DisplaySize.x * drawdata->FramebufferScale.x;
  float fb_height = drawdata->DisplaySize.y * drawdata->FramebufferScale.y;
  if (fb_width <= 0.0f && fb_height <= 0.0f)
    return;

  for (int i = 0; i < drawdata->CmdListsCount; ++i) {
    const auto* cmd_list = drawdata->CmdLists[i];

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert)),
                 static_cast<void*>(cmd_list->VtxBuffer.Data),
                 GL_STREAM_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx)),
                 static_cast<void*>(cmd_list->IdxBuffer.Data),
                 GL_STREAM_DRAW);

    for (const auto& pcmd : cmd_list->CmdBuffer) {
      if (pcmd.UserCallback) {
        if (pcmd.UserCallback == ImDrawCallback_ResetRenderState)
          SetupImRenderState();
        else
          pcmd.UserCallback(cmd_list, &pcmd);
      } else {
        float x1 = (pcmd.ClipRect.x - drawdata->DisplayPos.x) * drawdata->FramebufferScale.x;
        float y1 = (pcmd.ClipRect.y - drawdata->DisplayPos.y) * drawdata->FramebufferScale.y;
        float x2 = (pcmd.ClipRect.z - drawdata->DisplayPos.x) * drawdata->FramebufferScale.x;
        float y2 = (pcmd.ClipRect.w - drawdata->DisplayPos.y) * drawdata->FramebufferScale.y;
        if (x1 >= fb_width || y1 >= fb_height || x2 < 0.0f || y2 < 0.0f)
          return;

        glScissor(static_cast<GLint>(x1),
                  static_cast<GLint>(fb_height - y2),
                  static_cast<GLsizei>(x2 - x1),
                  static_cast<GLsizei>(y2 - y1));

        glBindTexture(GL_TEXTURE_2D, reinterpret_cast<intptr_t>(pcmd.GetTexID()));

        m_program.Vao(0).Bind();

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 static_cast<GLsizei>(pcmd.ElemCount),
                                 sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                 reinterpret_cast<void*>(pcmd.IdxOffset * sizeof(ImDrawIdx)),
                                 static_cast<GLint>(pcmd.VtxOffset));

        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(pcmd.ElemCount),
                       sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                       reinterpret_cast<void*>(pcmd.IdxOffset * sizeof(ImDrawIdx)));
      }
    }
  }
}

}  // namespace cheers
