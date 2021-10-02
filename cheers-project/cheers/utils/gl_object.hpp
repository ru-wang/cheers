#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include <GL/glew.h>

namespace cheers {

// struct VertexAttribute {
//   bool bIsEnabled = GL_FALSE;
//   int iSize = 4;  // This is the number of elements in this attribute, 1-4.
//   unsigned int iStride = 0;
//   VertexAttribType eType = GL_FLOAT;
//   bool bIsNormalized = GL_FALSE;
//   bool bIsIntegral = GL_FALSE;
//   void * pBufferObjectOffset = 0;
//   BufferObject * pBufferObj = 0;
// };
//
// struct VertexArrayObject {
//   BufferObject *pElementArrayBufferObject = NULL;
//   VertexAttribute attributes[GL_MAX_VERTEX_ATTRIB];
// }

template <GLenum Target>
class ArrayBufferObject {
public:
  ArrayBufferObject() { glGenBuffers(1, &m_name); }

  ArrayBufferObject(ArrayBufferObject&& other) {
    std::swap(m_name, other.m_name);
  }

  ~ArrayBufferObject() noexcept {
    if (m_name)
      glDeleteBuffers(1, &m_name);
  }

  void Bind() const { glBindBuffer(Target, m_name); }
  void Unbind() const { glBindBuffer(Target, 0); }

  GLuint name() const noexcept { return m_name; }

 protected:
  GLuint m_name = 0;
};

class Texture2dObject {
public:
  Texture2dObject() { glGenTextures(1, &m_name); }

  Texture2dObject(Texture2dObject&& other) {
    std::swap(m_name, other.m_name);
  }

  ~Texture2dObject() noexcept {
    if (m_name) glDeleteTextures(1, &m_name);
  }

  void Bind() const { glBindTexture(GL_TEXTURE_2D, m_name); }
  void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

  GLuint name() const noexcept { return m_name; }

private:
  GLuint m_name = 0;
};

class VertexAttribObject {
 public:
  VertexAttribObject();
  VertexAttribObject(VertexAttribObject&& other);
  ~VertexAttribObject() noexcept;

  void Bind() const;
  void Unbind() const;
  void GenEbo(size_t num);
  void GenVbo(size_t num);

  auto& Ebo(size_t index) const noexcept { return m_ebos.at(index); }
  auto& Vbo(size_t index) const noexcept { return m_vbos.at(index); }

  auto& Ebo(size_t index) noexcept { return m_ebos.at(index); }
  auto& Vbo(size_t index) noexcept { return m_vbos.at(index); }

  size_t EboSize() const noexcept { return m_ebos.size(); }
  size_t VboSize() const noexcept { return m_vbos.size(); }

  GLuint name() const noexcept { return m_name; }

 private:
  GLuint m_name = 0;
  std::vector<ArrayBufferObject<GL_ELEMENT_ARRAY_BUFFER>> m_ebos;
  std::vector<ArrayBufferObject<GL_ARRAY_BUFFER>> m_vbos;
};

class ShaderObject {
 public:
  ShaderObject(const std::filesystem::path& shader_path);
  ShaderObject(const std::basic_string<GLchar>& shader_text, GLenum shader_type);
  ~ShaderObject() noexcept;

  GLuint name() const noexcept { return m_name; }

 private:
  void LoadShader(const GLchar shader_text[], GLenum shader_type);

  GLuint m_name = 0;
};

}  // namespace cheers
