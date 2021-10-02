#include "task.hpp"

#include <chrono>
#include <thread>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "cheers/proto/image.pb.h"
#include "cheers/proto/misc.pb.h"
#include "cheers/proto/model.pb.h"
#include "cheers/proto/path.pb.h"
#include "cheers/window/window.hpp"
#include "interpolator.hpp"

namespace {

cheers::message::Pose MakePose(const SE3<float>& tf, bool with_orientation = true) {
  cheers::message::Pose pose;
  if (with_orientation) {
    pose.mutable_orientation()->set_x(tf[3]);
    pose.mutable_orientation()->set_y(tf[4]);
    pose.mutable_orientation()->set_z(tf[5]);
    pose.mutable_orientation()->set_w(tf[6]);
  }
  pose.mutable_position()->set_x(tf[0]);
  pose.mutable_position()->set_y(tf[1]);
  pose.mutable_position()->set_z(tf[2]);
  return pose;
}

cheers::message::Pose SimpleInterpolate(
    const SE3<float>& lhs, const SE3<float>& rhs, double alpha, bool with_orientation = true) {
  SE3<float> tf;
  tf.head<3>() = lhs.head<3>() + alpha * (rhs.head<3>() - lhs.head<3>());
  tf.tail<4>() =
      Eigen::Quaternionf(lhs.tail<4>()).slerp(alpha, Eigen::Quaternionf(rhs.tail<4>())).coeffs();

  cheers::message::Pose pose;
  if (with_orientation) {
    pose.mutable_orientation()->set_x(tf[3]);
    pose.mutable_orientation()->set_y(tf[4]);
    pose.mutable_orientation()->set_z(tf[5]);
    pose.mutable_orientation()->set_w(tf[6]);
  }
  pose.mutable_position()->set_x(tf[0]);
  pose.mutable_position()->set_y(tf[1]);
  pose.mutable_position()->set_z(tf[2]);
  return pose;
}

std::vector<cheers::message::Vector3f> RandomSpherePoints(int num_points, int radius) {
  std::vector<cheers::message::Vector3f> points(num_points);
  for (int i = 0; i < num_points; ++i) {
    Eigen::Vector3f point = Eigen::Vector3f::Random();
    if (point.norm() <= 1.0f) {
      points.at(i).set_x(radius * point.x());
      points.at(i).set_y(radius * point.y());
      points.at(i).set_z(radius * point.z());
    }
  }
  return points;
}

}  // namespace

void Task::WaitAndSleep(int milliseconds) const {
  cheers::Window::Instance().WaitForWindowStalled();
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

Interpolation::Interpolation(const PathLayerPtr& path_layer, int slerp_count)
    : path_layer(path_layer), m_slerp_count(slerp_count) {
  cheers::message::Pose cam_pose;
  cam_pose.mutable_orientation()->set_x(0.5);
  cam_pose.mutable_orientation()->set_y(-0.5);
  cam_pose.mutable_orientation()->set_z(0.5);
  cam_pose.mutable_orientation()->set_w(-0.5);

  cheers::message::PathConfig path_config;
  path_config.set_line_width(2.0f);
  path_config.mutable_sensor_pose()->CopyFrom(cam_pose);

  path_config.set_name("green");
  path_config.mutable_color()->set_r(0.0f);
  path_config.mutable_color()->set_g(1.0f);
  path_layer->CreatePath(path_config);

  path_config.set_name("red");
  path_config.mutable_color()->set_r(1.0f);
  path_config.mutable_color()->set_g(0.0f);
  path_layer->CreatePath(path_config);
}

void Interpolation::operator()() const noexcept {
  double step = 1.0 / m_slerp_count;
  auto tf_0 = SE3Identity<float>();

  while (!quit_all_tasks) {
    auto tf_1 = SE3Concat(tf_0, SE3Random<float>(1.0f));
    Interpolator slerp(tf_0, tf_1);

    for (int k = 0; k <= m_slerp_count && !quit_all_tasks; ++k) {
      if (k > 1) {
        path_layer->UpdateTailPose("green", MakePose(slerp((k - 1) * step), false));
        path_layer->UpdateTailPose("red", SimpleInterpolate(tf_0, tf_1, (k - 1) * step, false));
      }
      path_layer->AddPose("green", MakePose(slerp(k * step)));
      path_layer->AddPose("red", SimpleInterpolate(tf_0, tf_1, k * step));
      WaitAndSleep(10);
    }

    tf_0 = tf_1;
  }
}

RandomSphere::RandomSphere(
    const ModelLayerPtr& model_layer, int num_point_per_round, float sphere_radius)
    : model_layer(model_layer)
    , num_point_per_round(num_point_per_round)
    , sphere_radius(sphere_radius) {
  cheers::message::ModelConfig model_config;
  model_config.set_name("model");
  model_config.set_point_size(5.0f);
  model_config.mutable_color()->set_g(0.8f);
  model_config.mutable_color()->set_b(0.8f);
  model_config.mutable_color()->set_a(0.3f);
  model_layer->CreateModel(model_config);
}

void RandomSphere::operator()() const noexcept {
  while (!quit_all_tasks) {
    auto random_points = RandomSpherePoints(num_point_per_round, sphere_radius);
    model_layer->AddVertices("model", random_points.cbegin(), random_points.cend());
    WaitAndSleep(10);
  }
}

ImageDisplay::ImageDisplay(const RGBImageLayerPtr& image_layer,
                           std::vector<std::string> image_paths)
    : image_layer(image_layer), image_paths(std::move(image_paths)) {
  cheers::message::ImageHandlerConfig image_handler_config;
  image_handler_config.set_name("image");
  image_layer->CreateImagePlaceHolder(image_handler_config);
}

void ImageDisplay::operator()() const noexcept {
  int width = 0;
  int height = 0;

  for (auto it = image_paths.cbegin(); it != image_paths.cend() && !quit_all_tasks; ++it) {
    auto image_data = stbi_load(it->c_str(), &width, &height, nullptr, 3);
    if (image_data) {
      cheers::message::Image image;
      image.set_width(width);
      image.set_height(height);
      image.set_data(std::string(reinterpret_cast<char*>(image_data), width * height * 3));

      stbi_image_free(image_data);
      image_layer->UpdateImage("image", std::move(image));
    }
    WaitAndSleep(100);
  }
}
