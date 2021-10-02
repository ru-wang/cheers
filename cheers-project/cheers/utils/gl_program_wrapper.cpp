#include "cheers/utils/gl_program_wrapper.hpp"

namespace cheers {

ProgramWrapper::ProgramWrapper() : m_name(glCreateProgram()) {}

ProgramWrapper::~ProgramWrapper() noexcept {
  glDeleteProgram(m_name);
}

void ProgramWrapper::Link() const {
  glLinkProgram(m_name);
}

void ProgramWrapper::Use() const {
  glUseProgram(m_name);
}

GLuint ProgramWrapper::Uniform(const GLchar name[]) const {
  return glGetUniformLocation(m_name, name);
}

GLuint ProgramWrapper::Attrib(const GLchar name[]) const {
  return glGetAttribLocation(m_name, name);
}

void ProgramWrapper::GenVao(size_t num) {
  m_vaos.resize(m_vaos.size() + num);
}

void ProgramWrapper::GenText(size_t num) {
  m_texts.resize(m_texts.size() + num);
}

}  // namespace cheers
