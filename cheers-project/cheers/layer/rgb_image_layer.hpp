#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cheers/layer/layer.hpp"
#include "cheers/proto/image.pb.h"
#include "cheers/utils/gl_object.hpp"

namespace cheers {

class RGBImageLayer : public Layer {
public:
  RGBImageLayer() = default;
  RGBImageLayer(const std::string& title) : m_title(title) {}
  ~RGBImageLayer() noexcept override = default;

  int CreateImagePlaceHolder(const message::ImageHandlerConfig& config) {
    std::lock_guard lock(render_data_mutex());
    if (!m_render_data.name_index.count(config.name())) {
      m_render_data.name_index.emplace(config.name(), m_render_data.image_handler_messages.size());
      m_render_data.image_handler_messages.emplace_back().mutable_config()->CopyFrom(config);
      m_render_data.dirty = true;
    }
    return m_render_data.name_index.at(config.name());
  }

  void UpdateImage(const std::string& name, message::Image&& image) {
    std::lock_guard lock(render_data_mutex());
    int index = m_render_data.name_index.at(name);
    m_render_data.image_handler_messages.at(index).mutable_image()->Swap(&image);
    m_render_data.dirty = true;
  }

  void UpdateImage(int index, message::Image&& image) {
    std::lock_guard lock(render_data_mutex());
    m_render_data.image_handler_messages.at(index).mutable_image()->Swap(&image);
    m_render_data.dirty = true;
  }

private:
  struct RenderData {
    struct ImageData {
      unsigned short width = 0;
      unsigned short height = 0;
      std::string bytes;
    };

    bool dirty = false;
    std::unordered_map<std::string, int> name_index;
    std::vector<message::ImageHandler> image_handler_messages;

    // generated data
    std::vector<ImageData> image_data;
    std::vector<Texture2dObject> texture_data;
  };

  void OnDestroyRenderer() override;

  void OnUpdateImFrame() override;

  void UpdateImageData();

  const std::string m_title = "RGB Image Viewer";
  RenderData m_render_data;
};

}  // namespace cheers
