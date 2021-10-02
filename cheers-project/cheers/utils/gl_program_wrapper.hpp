#pragma once

#include <functional>
#include <vector>

#include <GL/glew.h>

#include "cheers/utils/gl_object.hpp"

namespace cheers {

class ProgramWrapper {
public:
  ProgramWrapper();
  ~ProgramWrapper() noexcept;

  template <typename... Args>
  void Attach(const Args&... shaders) const {
    for (auto shader : {std::ref(shaders)...})
      glAttachShader(m_name, shader.get().name());
  }

  template <typename... Args>
  void Detach(const Args&... shaders) const {
    for (auto shader : {std::ref(shaders)...})
      glDetachShader(m_name, shader.get().name());
  }

  template <typename... Args, size_t... N>
  void EnableAttrib(const Args (&... names)[N]) const {
    for (auto name : {names...})
      glEnableVertexAttribArray(Attrib(name));
  }

  template <typename... Args, size_t... N>
  void DisableAttrib(const Args (&... names)[N]) const {
    for (auto name : {names...})
      glDisableVertexAttribArray(Attrib(name));
  }

  void Link() const;
  void Use() const;

  GLuint Uniform(const GLchar name[]) const;
  GLuint Attrib(const GLchar name[]) const;

  void GenVao(size_t num);
  void GenText(size_t num);

  auto& Vao(size_t index) const noexcept { return m_vaos.at(index); }
  auto& Text(size_t index) const noexcept { return m_texts.at(index); }

  auto& Vao(size_t index) noexcept { return m_vaos.at(index); }
  auto& Text(size_t index) noexcept { return m_texts.at(index); }

  size_t VaoSize() const noexcept { return m_vaos.size(); }
  size_t TextSize() const noexcept { return m_texts.size(); }

private:
  const GLuint m_name = 0;
  std::vector<VertexAttribObject> m_vaos;
  std::vector<Texture2dObject> m_texts;
};

}  // namespace cheers
