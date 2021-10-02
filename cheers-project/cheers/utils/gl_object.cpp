#include "cheers/utils/gl_object.hpp"

#include <fstream>
#include <sstream>

#include "cheers/utils/throw_and_check.hpp"

namespace cheers {

VertexAttribObject::VertexAttribObject() {
  glGenVertexArrays(1, &m_name);
}

VertexAttribObject::VertexAttribObject(VertexAttribObject&& other) {
  std::swap(m_name, other.m_name);
  m_ebos.swap(other.m_ebos);
  m_vbos.swap(other.m_vbos);
}

VertexAttribObject::~VertexAttribObject() noexcept {
  if (m_name)
    glDeleteVertexArrays(1, &m_name);
}

void VertexAttribObject::Bind() const {
  glBindVertexArray(m_name);
}

void VertexAttribObject::Unbind() const {
  glBindVertexArray(0);
}

void VertexAttribObject::GenEbo(size_t num) {
  m_ebos.resize(m_ebos.size() + num);
}

void VertexAttribObject::GenVbo(size_t num) {
  m_vbos.resize(m_vbos.size() + num);
}

ShaderObject::ShaderObject(const std::filesystem::path& shader_path) {
  CHECK(std::filesystem::is_regular_file(shader_path), "{:10}:{} | invalid shader path {}",
        __FILE__, __LINE__, shader_path.string());

  if (shader_path.extension() == ".vert") {
    std::ifstream ifs(shader_path);
    std::basic_ostringstream<GLchar> oss;
    oss << ifs.rdbuf();
    LoadShader(oss.str().c_str(), GL_VERTEX_SHADER);
  } else if (shader_path.extension() == ".frag") {
    std::ifstream ifs(shader_path);
    std::basic_ostringstream<GLchar> oss;
    oss << ifs.rdbuf();
    LoadShader(oss.str().c_str(), GL_FRAGMENT_SHADER);
  } else {
    THROW("{:10}:{} | invalid shader name {}", __FILE__, __LINE__, shader_path.string());
  }
}

ShaderObject::ShaderObject(const std::basic_string<GLchar>& shader_text, GLenum shader_type) {
  LoadShader(shader_text.c_str(), shader_type);
}

ShaderObject::~ShaderObject() noexcept {
  glDeleteShader(m_name);
}

void ShaderObject::LoadShader(const GLchar shader_text[], GLenum shader_type) {
  m_name = glCreateShader(shader_type);
  glShaderSource(m_name, 1, &shader_text, nullptr);
  glCompileShader(m_name);

  GLint compile_status;
  glGetShaderiv(m_name, GL_COMPILE_STATUS, &compile_status);

  if (compile_status == GL_FALSE) {
    GLint infolog_len;
    glGetShaderiv(m_name, GL_INFO_LOG_LENGTH, &infolog_len);

    std::basic_string<GLchar> infolog(infolog_len + 1, 0);
    glGetShaderInfoLog(m_name, infolog_len, nullptr, infolog.data());
    THROW("{:10}:{} | {}", __FILE__, __LINE__, infolog);
  }
}

}  // namespace cheers
