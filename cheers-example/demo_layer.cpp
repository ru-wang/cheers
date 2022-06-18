#include "demo_layer.hpp"

#include <cmath>
#include <algorithm>
#include <filesystem>
#include <iterator>

#include <Eigen/Eigen>
#include <GL/glew.h>
#include <glm/ext.hpp>
#include <imgui.h>

#define FLOAT_ATTRIB_POINTER(prog, name, size, stride, offset) \
  render_program(prog).EnableAttrib(#name);                    \
  glVertexAttribPointer(render_program(prog).Attrib(#name),    \
                        size,                                  \
                        GL_FLOAT,                              \
                        GL_FALSE,                              \
                        sizeof(float) * stride,                \
                        reinterpret_cast<const void*>(sizeof(float) * offset))

DemoLayer::DemoLayer(size_t max_num_object) {
  Eigen::Isometry3f extrinsics = Eigen::Isometry3f::Identity();
  extrinsics.prerotate(Eigen::AngleAxisf(M_PI_4, Eigen::Vector3f::UnitX()));
  extrinsics.pretranslate(Eigen::Vector3f(0.0f, 0.0f, RenderData::ROTATION_RADIUS));

  m_render_data.extrinsics = glm::make_mat4(extrinsics.data());

  for (size_t i = 0; i < max_num_object; ++i) {
    auto& local_object = m_render_data.local_objects.emplace_back();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::mat4> poses;

    for (size_t j = 0; j < RenderData::NUM_VERTICES_PER_OBJECT; ++j) {
      Eigen::Vector3f vertex = Eigen::Vector3f::Random();

      Eigen::Vector3f color;
      color.x() = i * 1.0f / max_num_object;
      color.y() = vertex.head<2>().norm() / M_SQRT2;
      color.z() = (vertex.z() + 1.0f) / 2.0f;
      ImGui::ColorConvertHSVtoRGB(color.x(), color.y(), color.z(), color.x(), color.y(), color.z());

      Eigen::Isometry3f pose = Eigen::Isometry3f::Identity();
      pose.rotate(Eigen::AngleAxisf(M_PI * 2.0f * i / max_num_object, Eigen::Vector3f::UnitZ()));
      pose.translate(Eigen::Vector3f(RenderData::ROTATION_RADIUS, 0.0f, 0.0f));

      vertices.push_back(glm::make_vec3(vertex.data()));
      colors.push_back(glm::make_vec3(color.data()));
      poses.push_back(glm::make_mat4(pose.matrix().data()));
    }

    local_object.vertex.swap(vertices);
    local_object.color.swap(colors);
    local_object.pose.swap(poses);
  }
}

void DemoLayer::OnUpdateRenderData() {
  m_render_data.bufferdata.clear();
  m_render_data.bufferdata.reserve(m_render_data.local_objects.size());

  for (const auto& local_object : m_render_data.local_objects) {
    auto& [num_points, buffer] = m_render_data.bufferdata.emplace_back();
    num_points = local_object.vertex.size();

    for (size_t i = 0; i < local_object.vertex.size(); ++i) {
      const auto& vertex = local_object.vertex.at(i);
      const auto& color = local_object.color.at(i);
      const auto& pose = local_object.pose.at(i);
      std::copy(glm::value_ptr(vertex), glm::value_ptr(vertex) + 3, std::back_inserter(buffer));
      std::copy(glm::value_ptr(color), glm::value_ptr(color) + 3, std::back_inserter(buffer));
      std::copy(glm::value_ptr(pose), glm::value_ptr(pose) + 16, std::back_inserter(buffer));
    }
  }
}

void DemoLayer::OnRenderLayer(const float* matrix_vp) {
  cheers::ShaderObject vshader(std::filesystem::path("cheers-example/demo_shader_450.vert"));
  cheers::ShaderObject fshader(std::filesystem::path("cheers-example/demo_shader_450.frag"));

  add_render_program();
  render_program(0).Attach(vshader, fshader);
  render_program(0).Link();
  render_program(0).Detach(vshader, fshader);

  render_program(0).Use();

  glm::mat4 proj(1.0f);
  glUniformMatrix4fv(render_program(0).Uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(render_program(0).Uniform("mv_matrix"), 1, GL_FALSE, matrix_vp);
  glUniformMatrix4fv(render_program(0).Uniform("extrinsics"), 1, GL_FALSE,
                     glm::value_ptr(m_render_data.extrinsics));

  render_program(0).GenVao(1);
  render_program(0).Vao(0).Bind();
  for (size_t i = 0; i < m_render_data.bufferdata.size(); ++i) {
    const auto& [num_points, buffer] = m_render_data.bufferdata.at(i);

    render_program(0).Vao(0).GenVbo(1);
    render_program(0).Vao(0).Vbo(i).Bind();
    {
      GLsizeiptr data_size = sizeof(float) * buffer.size();
      const void* data_ptr = buffer.data();
      glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);

      FLOAT_ATTRIB_POINTER(0, vertex, 3, 22, 0);
      FLOAT_ATTRIB_POINTER(0, color, 3, 22, 3);
      FLOAT_ATTRIB_POINTER(0, rotation_c0, 4, 22, 6);
      FLOAT_ATTRIB_POINTER(0, rotation_c1, 4, 22, 10);
      FLOAT_ATTRIB_POINTER(0, rotation_c2, 4, 22, 14);
      FLOAT_ATTRIB_POINTER(0, translation, 4, 22, 18);

      glPointSize(RenderData::POINT_SIZE);
      glDrawArrays(GL_POINTS, 0, num_points);
    }
    render_program(0).Vao(0).Vbo(i).Unbind();
  }
  render_program(0).Vao(0).Unbind();

  clear_render_program();
}

#undef FLOAT_ATTRIB_POINTER
