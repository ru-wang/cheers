#include "cheers/utils/gl_backup_state.hpp"

namespace cheers {

GlBackupState::GlBackupState() {
  glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&m_active_texture));

  glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&m_program));
  glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&m_texture));
  glGetIntegerv(GL_SAMPLER_BINDING, reinterpret_cast<GLint*>(&m_sampler));
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&m_array_buffer));
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&m_vertex_array_object));

  glGetIntegerv(GL_POLYGON_MODE, m_polygon_mode.data());
  glGetIntegerv(GL_VIEWPORT, m_viewport.data());
  glGetIntegerv(GL_SCISSOR_BOX, m_scissor_box.data());

  glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint*>(&m_blend_src_rgb));
  glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint*>(&m_blend_dst_rgb));
  glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&m_blend_src_alpha));
  glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&m_blend_dst_alpha));
  glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint*>(&m_blend_equation_rgb));
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint*>(&m_blend_equation_alpha));

  m_enable_blend = glIsEnabled(GL_BLEND);
  m_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  m_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  m_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
  m_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
  m_enable_primitive_restart = glIsEnabled(GL_PRIMITIVE_RESTART);
}

GlBackupState::~GlBackupState() noexcept {
  glActiveTexture(m_active_texture);

  glUseProgram(m_program);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glBindSampler(0, m_sampler);
  glBindVertexArray(m_vertex_array_object);
  glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
  glBlendEquationSeparate(m_blend_equation_rgb, m_blend_equation_alpha);

  glPolygonMode(GL_FRONT_AND_BACK, m_polygon_mode[0]);
  glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
  glScissor(m_scissor_box[0], m_scissor_box[1], m_scissor_box[2], m_scissor_box[3]);

  glBlendFuncSeparate(m_blend_src_rgb, m_blend_dst_rgb, m_blend_src_alpha, m_blend_dst_alpha);

  m_enable_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  m_enable_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  m_enable_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  m_enable_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
  m_enable_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
  m_enable_primitive_restart ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
}

}  // namespace cheers
