#pragma once

#include <array>

#include <GL/glew.h>

namespace cheers {

class GlBackupState {
public:
  GlBackupState();
  ~GlBackupState() noexcept;

private:
  GLenum m_active_texture;

  GLuint m_program;
  GLuint m_texture;
  GLuint m_sampler;
  GLuint m_array_buffer;
  GLuint m_vertex_array_object;

  std::array<GLint, 2> m_polygon_mode;
  std::array<GLint, 4> m_viewport;
  std::array<GLint, 4> m_scissor_box;

  GLenum m_blend_src_rgb;
  GLenum m_blend_dst_rgb;
  GLenum m_blend_src_alpha;
  GLenum m_blend_dst_alpha;
  GLenum m_blend_equation_rgb;
  GLenum m_blend_equation_alpha;

  GLboolean m_enable_blend;
  GLboolean m_enable_cull_face;
  GLboolean m_enable_depth_test;
  GLboolean m_enable_stencil_test;
  GLboolean m_enable_scissor_test;
  GLboolean m_enable_primitive_restart;
};

}  // namespace cheers
