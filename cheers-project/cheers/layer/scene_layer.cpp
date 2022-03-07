#include "cheers/layer/scene_layer.hpp"

#include <algorithm>
#include <iterator>

#include <GL/glew.h>
#include <imgui.h>

#include "cheers/shader/shader_absolute_path.hpp"
#include "cheers/utils/gl_object.hpp"

namespace cheers {

namespace {

constexpr std::array AXIS_DATA = {
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // O(red)
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // X(red)
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // O(green)
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Y(green)
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // O(blue)
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Z(blue)
};

}

void SceneLayer::SetGridUpAxis(int up_axis) {
  if (up_axis == 0 || up_axis == 1 || up_axis == 2)
    m_render_data.grid_up_axis = up_axis;
}

void SceneLayer::OnCreateRenderer() {
  add_render_program();
  add_render_program();
  InitAxisProgram();
  InitGridProgram();
}

void SceneLayer::OnDestroyRenderer() {
  clear_render_program();
}

void SceneLayer::OnUpdateImFrame() {
  ImGui::Begin("Scene Control Panel");
  ImGui::Text("Render Rate %.1f FPS", ImGui::GetIO().Framerate);

  ImGui::ColorEdit3("Background", m_render_data.background_color.data());
  ImGui::ColorEdit3("Grid Color", m_render_data.grid_color.data());

  ImGui::DragInt("Grids Per Quadrant", &m_render_data.grid_num_per_quad, 1.0f, 10, 100);
  ImGui::DragFloat("Grid Size (m)", &m_render_data.grid_metric_size, 0.1f, 0.1f, 10.0f, "%.1f");

  ImGui::End();
}

void SceneLayer::OnUpdateRenderData() {
  m_render_data.grid_data.clear();
  for (int i = -m_render_data.grid_num_per_quad; i <= m_render_data.grid_num_per_quad; ++i) {
    std::array<int, 12> vertices;
    switch (m_render_data.grid_up_axis) {
      case 0:
        vertices = {
            0, i, +m_render_data.grid_num_per_quad,  // a
            0, i, -m_render_data.grid_num_per_quad,  // b
            0, +m_render_data.grid_num_per_quad, i,  // c
            0, -m_render_data.grid_num_per_quad, i,  // d
        };
        break;
      case 1:
        vertices = {
            i, 0, +m_render_data.grid_num_per_quad,  // a
            i, 0, -m_render_data.grid_num_per_quad,  // b
            +m_render_data.grid_num_per_quad, 0, i,  // c
            -m_render_data.grid_num_per_quad, 0, i,  // d
        };
        break;
      case 2:
        vertices = {
            i, +m_render_data.grid_num_per_quad, 0,  // a
            i, -m_render_data.grid_num_per_quad, 0,  // b
            +m_render_data.grid_num_per_quad, i, 0,  // c
            -m_render_data.grid_num_per_quad, i, 0,  // d
        };
        break;
      default:
        break;
    }
    std::move(vertices.begin(), vertices.end(), std::back_inserter(m_render_data.grid_data));
  }
}

void SceneLayer::OnRenderLayer(const float* matrix_vp) {
  glClearColor(m_render_data.background_color.at(0), m_render_data.background_color.at(1),
               m_render_data.background_color.at(2), 1.0f);
  RenderAxis(matrix_vp);
  RenderGrid(matrix_vp);
}

void SceneLayer::InitAxisProgram() {
  ShaderObject vshader(axis_vshader_absolute_path);
  ShaderObject fshader(axis_fshader_absolute_path);
  render_program(0).Attach(vshader, fshader);
  render_program(0).Link();
  render_program(0).Detach(vshader, fshader);

  render_program(0).GenVao(1);
  render_program(0).Vao(0).Bind();
  render_program(0).EnableAttrib("pos", "clr");

  render_program(0).Vao(0).GenVbo(1);
  render_program(0).Vao(0).Vbo(0).Bind();

  GLsizei stride = sizeof(float) * 6;
  const void* clr_offset = reinterpret_cast<void*>(sizeof(float) * 3);
  glVertexAttribPointer(render_program(0).Attrib("pos"), 3, GL_FLOAT, GL_FALSE, stride, 0);
  glVertexAttribPointer(render_program(0).Attrib("clr"), 3, GL_FLOAT, GL_FALSE, stride, clr_offset);

  GLsizeiptr data_size = sizeof(float) * AXIS_DATA.size();
  const void* data_ptr = AXIS_DATA.data();
  glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);
}

void SceneLayer::InitGridProgram() {
  ShaderObject vshader(grid_vshader_absolute_path);
  ShaderObject fshader(grid_fshader_absolute_path);
  render_program(1).Attach(vshader, fshader);
  render_program(1).Link();
  render_program(1).Detach(vshader, fshader);

  render_program(1).GenVao(1);
  render_program(1).Vao(0).Bind();
  render_program(1).EnableAttrib("pos");

  render_program(1).Vao(0).GenVbo(1);
  render_program(1).Vao(0).Vbo(0).Bind();
  glVertexAttribIPointer(render_program(1).Attrib("pos"), 3, GL_INT, 0, 0);
}

void SceneLayer::RenderAxis(const float* matrix_vp) const {
  render_program(0).Use();

  glLineWidth(3.0f);
  glUniformMatrix4fv(render_program(0).Uniform("mvp"), 1, GL_FALSE, matrix_vp);
  glUniform1f(render_program(0).Uniform("metric"), m_render_data.grid_metric_size);

  render_program(0).Vao(0).Bind();
  glDrawArrays(GL_LINES, 0, 6);
}

void SceneLayer::RenderGrid(const float* matrix_vp) const {
  render_program(1).Use();

  glLineWidth(0.5f);
  glUniformMatrix4fv(render_program(1).Uniform("mvp"), 1, GL_FALSE, matrix_vp);
  glUniform1f(render_program(1).Uniform("metric"), m_render_data.grid_metric_size);
  glUniform3fv(render_program(1).Uniform("clr"), 1, m_render_data.grid_color.data());

  render_program(1).Vao(0).Bind();
  render_program(1).Vao(0).Vbo(0).Bind();
  GLsizeiptr data_size = sizeof(int) * m_render_data.grid_data.size();
  const void* data_ptr = m_render_data.grid_data.data();
  glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);

  GLsizei draw_index_count = m_render_data.grid_data.size() / 3;
  glDrawArrays(GL_LINES, 0, draw_index_count);
}

}  // namespace cheers
