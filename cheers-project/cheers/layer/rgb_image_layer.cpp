#include "cheers/layer/rgb_image_layer.hpp"

#include <GL/glew.h>
#include <imgui.h>

namespace cheers {

void RGBImageLayer::OnDestroyRenderer() {
  m_render_data.texture_data.clear();
}

void RGBImageLayer::OnUpdateImFrame() {
  UpdateImageData();

  if (m_render_data.image_data.empty())
    return;
  m_render_data.texture_data.resize(m_render_data.image_data.size());

  ImGui::Begin(m_title.c_str(), nullptr);

  float thumb_h = ImGui::GetContentRegionAvail().y;
  for (size_t i = 0; i < m_render_data.image_data.size(); ++i) {
    auto& image = m_render_data.image_data.at(i);
    auto& texture = m_render_data.texture_data.at(i);

    texture.Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    GLsizei width = image.width;
    GLsizei height = image.height;
    const void* data_ptr = image.bytes.data();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_ptr);

    float thumb_w = width * thumb_h / height;

    auto texture_name = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture.name()));
    ImGui::Image(texture_name, ImVec2(thumb_w, thumb_h));
    if (thumb_h < height && ImGui::IsWindowFocused() && ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Image(texture_name, ImVec2(width, height));
      ImGui::EndTooltip();
    }
    ImGui::SameLine();
  }

  ImGui::End();
}

void RGBImageLayer::UpdateImageData() {
  std::unique_lock try_lock(render_data_mutex(), std::try_to_lock);
  if (!try_lock.owns_lock())
    return;

  if (m_render_data.dirty) {
    m_render_data.image_data.resize(m_render_data.image_handler_messages.size());

    for (size_t i = 0; i < m_render_data.image_handler_messages.size(); ++i) {
      const auto& message = m_render_data.image_handler_messages.at(i);

      auto& image_data = m_render_data.image_data.at(i);
      image_data.width = message.image().width();
      image_data.height = message.image().height();
      image_data.bytes = message.image().data();
    }

    m_render_data.dirty = false;
  }
}

}  // namespace cheers
