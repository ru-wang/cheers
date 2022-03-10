#include "cheers/layer/path_layer.hpp"

#include <algorithm>
#include <utility>

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "cheers/layer/utils.hpp"
#include "cheers/resource/shader_absolute_path.hpp"
#include "cheers/utils/gl_object.hpp"
#include "cheers/utils/throw_and_check.hpp"

namespace cheers {

namespace {

constexpr std::array CAMERA_SHAPE = {
    +0.2f, +0.1f, 0.2f,  // right-bottom
    +0.2f, -0.1f, 0.2f,  // right-top

    +0.2f, -0.1f, 0.2f,  // right-top
    -0.2f, -0.1f, 0.2f,  // left-top

    -0.2f, -0.1f, 0.2f,  // left-top
    -0.2f, +0.1f, 0.2f,  // left-bottom

    -0.2f, +0.1f, 0.2f,  // left-bottom
    +0.2f, +0.1f, 0.2f,  // right-bottom

    +0.0f, +0.0f, 0.0f,  // center
    +0.2f, +0.1f, 0.2f,  // right-bottom

    +0.0f, +0.0f, 0.0f,  // center
    +0.2f, -0.1f, 0.2f,  // right-top

    +0.0f, +0.0f, 0.0f,  // center
    -0.2f, -0.1f, 0.2f,  // left-top

    +0.0f, +0.0f, 0.0f,  // center
    -0.2f, +0.1f, 0.2f,  // left-bottom

    +0.0f, +0.0f, 0.0f,  // center
    +0.0f, +0.3f, 0.0f,  // down
};

}

void PathLayer::OnCreateRenderer() {
  ShaderObject vshader(path_vshader_absolute_path);
  ShaderObject fshader(path_fshader_absolute_path);
  add_render_program();
  render_program(0).Attach(vshader, fshader);
  render_program(0).Link();
  render_program(0).Detach(vshader, fshader);
}

void PathLayer::OnDestroyRenderer() {
  clear_render_program();
}

void PathLayer::OnUpdateRenderData() {
  std::unique_lock try_lock(render_data_mutex(), std::try_to_lock);
  if (!try_lock.owns_lock())
    return;

  if (m_render_data.dirty) {
    AugmentVaoData();
    UpdatePathData();
    m_render_data.dirty = false;
  }
}

void PathLayer::OnRenderLayer(const float* matrix_vp) {
  render_program(0).Use();

  // draw path
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glUniformMatrix4fv(render_program(0).Uniform("mvp"), 1, GL_FALSE, matrix_vp);

  for (size_t i = 0; i < render_program(0).VaoSize(); ++i) {
    const auto& pathdata = m_render_data.path_data.at(i);
    glUniform4fv(render_program(0).Uniform("clr"), 1, pathdata.color.data());

    render_program(0).Vao(i).Bind();
    render_program(0).Vao(i).Vbo(VBO_PATH_DATA).Bind();
    glVertexAttribPointer(render_program(0).Attrib("pos"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLsizeiptr data_size = sizeof(float) * pathdata.vertex_data.size();
    const void* data_ptr = pathdata.vertex_data.data();
    glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);

    glLineWidth(pathdata.line_width);
    GLsizei draw_index_count = pathdata.vertex_data.size() / 3;
    glDrawArrays(GL_LINE_STRIP, 0, draw_index_count);
  }

  // draw cameras
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  auto vp_matrix = glm::make_mat4(matrix_vp);
  for (size_t i = 0; i < render_program(0).VaoSize(); ++i) {
    const auto& pathdata = m_render_data.path_data.at(i);
    glUniform4fv(render_program(0).Uniform("clr"), 1, pathdata.color.data());

    render_program(0).Vao(i).Bind();
    render_program(0).Vao(i).Vbo(VBO_NODE_DATA).Bind();
    glVertexAttribPointer(render_program(0).Attrib("pos"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glLineWidth(pathdata.line_width);
    auto sensor_matrix = glm::make_mat4(pathdata.sensor_pose.data());
    for (size_t offset = 0; offset < pathdata.pose_data.size(); offset += 16) {
      auto pose_matrix = glm::make_mat4(pathdata.pose_data.data() + offset) * sensor_matrix;
      auto mvp_matrix = vp_matrix * pose_matrix;
      glUniformMatrix4fv(render_program(0).Uniform("mvp"), 1, GL_FALSE, &mvp_matrix[0][0]);

      GLsizei draw_index_count = CAMERA_SHAPE.size() / 3;
      glDrawArrays(GL_LINES, 0, draw_index_count);
    }
  }
}

void PathLayer::AugmentVaoData() {
  size_t new_vao_size = m_render_data.path_messages.size();
  size_t old_vao_size = render_program(0).VaoSize();
  CHECK(new_vao_size >= old_vao_size, "{:10}:{} | {}", __FILE__, __LINE__, "wrong VAO size");
  if (new_vao_size == old_vao_size)
    return;

  render_program(0).GenVao(new_vao_size - old_vao_size);
  for (size_t i = old_vao_size; i < new_vao_size; ++i) {
    render_program(0).Vao(i).Bind();
    render_program(0).EnableAttrib("pos");

    render_program(0).Vao(i).GenVbo(VBO_COUNT);
    render_program(0).Vao(i).Vbo(VBO_NODE_DATA).Bind();

    GLsizeiptr data_size = sizeof(float) * CAMERA_SHAPE.size();
    const void* data_ptr = CAMERA_SHAPE.data();
    glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, GL_STATIC_DRAW);
  }
}

void PathLayer::UpdatePathData() {
  // update path config
  for (size_t i = m_render_data.path_data.size(); i < m_render_data.path_messages.size(); ++i) {
    const auto& config = m_render_data.path_messages.at(i).config();
    const auto& color = config.color();

    auto& pathdata = m_render_data.path_data.emplace_back();
    if (config.line_width() != 0.0f)
      pathdata.line_width = config.line_width();
    pathdata.color = {color.r(), color.g(), color.b(), color.a()};

    glm::mat4 mat(1.0f);
    if (config.has_sensor_pose()) {
      const auto& sensor_pose = config.sensor_pose();
      if (sensor_pose.has_orientation() && sensor_pose.has_position())
        mat = MakeTransform(sensor_pose.orientation(), sensor_pose.position());
      else if (sensor_pose.has_orientation())
        mat = MakeTransform(sensor_pose.orientation(), glm::vec3(0.0f));
      else if (sensor_pose.has_position())
        mat = MakeTransform(glm::identity<glm::quat>(), sensor_pose.position());
    }
    std::copy(&mat[0][0], &mat[0][0] + 16, pathdata.sensor_pose.begin());
  }

  // update path data
  for (size_t i = 0; i < m_render_data.path_messages.size(); ++i) {
    const auto& message = m_render_data.path_messages.at(i);
    auto& pathdata = m_render_data.path_data.at(i);

    pathdata.vertex_data.clear();
    pathdata.pose_data.clear();

    for (const auto& pose : message.pose()) {
      pathdata.vertex_data.push_back(pose.position().x());
      pathdata.vertex_data.push_back(pose.position().y());
      pathdata.vertex_data.push_back(pose.position().z());

      if (pose.has_orientation()) {
        auto mat = MakeTransform(pose.orientation(), pose.position());
        pathdata.pose_data.insert(pathdata.pose_data.end(), &mat[0][0], &mat[0][0] + 16);
      }
    }
  }
}

}  // namespace cheers
