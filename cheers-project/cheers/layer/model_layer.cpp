#include "cheers/layer/model_layer.hpp"

#include <GL/glew.h>

#include "cheers/shader/shader_absolute_path.hpp"
#include "cheers/utils/gl_object.hpp"
#include "cheers/utils/throw_and_check.hpp"

namespace cheers {

void ModelLayer::OnCreateRenderer() {
  ShaderObject vshader(model_vshader_absolute_path);
  ShaderObject fshader(model_fshader_absolute_path);
  add_render_program();
  render_program(0).Attach(vshader, fshader);
  render_program(0).Link();
  render_program(0).Detach(vshader, fshader);
}

void ModelLayer::OnDestroyRenderer() {
  clear_render_program();
}

void ModelLayer::OnUpdateRenderData() {
  std::unique_lock try_lock(render_data_mutex(), std::try_to_lock);
  if (!try_lock.owns_lock())
    return;

  if (m_render_data.dirty) {
    AugmentVaoData();
    UpdateModelData();
    m_render_data.dirty = false;
  }
}

void ModelLayer::OnRenderLayer(const float* matrix_vp) {
  render_program(0).Use();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glUniformMatrix4fv(render_program(0).Uniform("mvp"), 1, GL_FALSE, matrix_vp);
  for (size_t i = 0; i < render_program(0).VaoSize(); ++i) {
    const auto& modeldata = m_render_data.model_data.at(i);
    glUniform4fv(render_program(0).Uniform("clr"), 1, modeldata.color.data());

    render_program(0).Vao(i).Bind();
    render_program(0).Vao(i).Vbo(0).Bind();
    render_program(0).Vao(i).Ebo(0).Bind();
    glVertexAttribPointer(render_program(0).Attrib("pos"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    // copy point data
    GLsizeiptr data_size = sizeof(float) * modeldata.vertex_data.size();
    const void* data_ptr = modeldata.vertex_data.data();
    glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);

    // copy triangle data
    GLsizeiptr index_size = sizeof(short) * modeldata.index_data.size();
    const void* index_ptr = modeldata.index_data.data();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_ptr, GL_STATIC_DRAW);

    glPointSize(modeldata.point_size);
    if (index_size > 0) {
      GLsizei draw_element_index_count = modeldata.index_data.size() / 3;
      glDrawElements(GL_TRIANGLES, draw_element_index_count, GL_UNSIGNED_SHORT, 0);
    } else {
      GLsizei draw_index_count = modeldata.vertex_data.size() / 3;
      glDrawArrays(GL_POINTS, 0, draw_index_count);
    }
  }
}

void ModelLayer::AugmentVaoData() {
  size_t new_vao_size = m_render_data.model_messages.size();
  size_t old_vao_size = render_program(0).VaoSize();
  CHECK(new_vao_size >= old_vao_size, "{:10}:{} | {}", __FILE__, __LINE__, "wrong VAO size");
  if (new_vao_size == old_vao_size)
    return;

  render_program(0).GenVao(new_vao_size - old_vao_size);
  for (size_t i = old_vao_size; i < new_vao_size; ++i) {
    render_program(0).Vao(i).Bind();
    render_program(0).EnableAttrib("pos");

    render_program(0).Vao(i).GenVbo(1);
    render_program(0).Vao(i).GenEbo(1);
    render_program(0).Vao(i).Vbo(0).Bind();
    render_program(0).Vao(i).Ebo(0).Bind();
  }
}

void ModelLayer::UpdateModelData() {
  // update model config
  for (size_t i = m_render_data.model_data.size(); i < m_render_data.model_messages.size(); ++i) {
    const auto& config = m_render_data.model_messages.at(i).config();
    const auto& color = config.color();

    auto& modeldata = m_render_data.model_data.emplace_back();
    if (config.point_size() != 0.0f)
      modeldata.point_size = config.point_size();
    modeldata.color = {color.r(), color.g(), color.b(), color.a()};
  }

  // update model data
  for (size_t i = 0; i < m_render_data.model_messages.size(); ++i) {
    const auto& message = m_render_data.model_messages.at(i);
    auto& modeldata = m_render_data.model_data.at(i);

    modeldata.vertex_data.clear();
    for (const auto& vertex : message.vertex()) {
      modeldata.vertex_data.push_back(vertex.x());
      modeldata.vertex_data.push_back(vertex.y());
      modeldata.vertex_data.push_back(vertex.z());
    }

    modeldata.index_data.clear();
    modeldata.normal_data.clear();
    for (const auto& triangle : message.triangle()) {
      modeldata.vertex_data.push_back(triangle.index().x());
      modeldata.vertex_data.push_back(triangle.index().y());
      modeldata.vertex_data.push_back(triangle.index().z());
      modeldata.normal_data.push_back(triangle.normal().x());
      modeldata.normal_data.push_back(triangle.normal().y());
      modeldata.normal_data.push_back(triangle.normal().z());
    }
  }
}

}  // namespace cheers
