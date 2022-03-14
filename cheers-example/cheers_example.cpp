#include <algorithm>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include "cheers/layer/model_layer.hpp"
#include "cheers/layer/path_layer.hpp"
#include "cheers/layer/rgb_image_layer.hpp"
#include "cheers/utils/im_export.hpp"
#include "cheers/window/window.hpp"
#include "task.hpp"

class PlotLayer final : public cheers::Layer {
public:
  ~PlotLayer() noexcept override = default;

private:
  void OnUpdateImFrame() override {
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
  }
};

int main(int args, char* argv[]) {
  using namespace cheers;

  std::vector<std::string> imgs;
  if (args > 1) {
    for (const auto& entry : std::filesystem::directory_iterator(argv[1]))
      imgs.push_back(entry.path());
    std::sort(imgs.begin(), imgs.end());
  }

  auto path_layer = Window::Instance().InstallSharedLayer<PathLayer>();
  auto model_layer = Window::Instance().InstallSharedLayer<ModelLayer>();
  auto image_layer = Window::Instance().InstallSharedLayer<RGBImageLayer>();
  Window::Instance().InstallSharedLayer<PlotLayer>();

  auto runner1 = std::thread(Interpolation(path_layer, 100));
  auto runner2 = std::thread(RandomSphere(model_layer, 10, 10.0f));
  auto runner3 = std::thread(ImageDisplay(image_layer, imgs));

#ifndef CHEERS_EXAMPLE_MULTITHREAD
  Window::Instance().CreateContext();
  while (Window::Instance().WaitForWindowExiting())
    ;
  Window::Instance().DestroyContext();
#else
  std::thread{[]() {
    Window::Instance().CreateContext();
    while (Window::Instance().WaitForWindowExiting())
      ;
    Window::Instance().DestroyContext();
  }}.join();
#endif

  Task::quit_all_tasks = true;
  runner1.join();
  runner2.join();
  runner3.join();

  return 0;
}
