#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <stb_image.h>

#include "cheers/layer/model_layer.hpp"
#include "cheers/layer/path_layer.hpp"
#include "cheers/layer/rgb_image_layer.hpp"

class Task {
public:
  virtual void operator()() const noexcept = 0;

  inline static std::atomic_bool quit_all_tasks = false;

protected:
  void WaitAndSleep(int milliseconds) const;
};

class Interpolation : Task {
public:
  using PathLayerPtr = std::shared_ptr<cheers::PathLayer>;
  Interpolation(const PathLayerPtr& path_layer, int slerp_count);
  void operator()() const noexcept override;

private:
  const PathLayerPtr path_layer;
  const int m_slerp_count;
};

class RandomSphere : Task {
public:
  using ModelLayerPtr = std::shared_ptr<cheers::ModelLayer>;
  RandomSphere(const ModelLayerPtr& model_layer, int num_point_per_round, float sphere_radius);
  void operator()() const noexcept override;

private:
  const ModelLayerPtr model_layer;
  const int num_point_per_round;
  const float sphere_radius;
};

class ImageDisplay : Task {
public:
  using RGBImageLayerPtr = std::shared_ptr<cheers::RGBImageLayer>;
  ImageDisplay(const RGBImageLayerPtr& image_layer, std::vector<std::string> image_paths);
  void operator()() const noexcept override;

private:
  const RGBImageLayerPtr image_layer;
  const std::vector<std::string> image_paths;
};
