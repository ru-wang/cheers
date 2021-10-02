#include "cheers/layer/widget.hpp"

#include <imgui.h>

namespace cheers::widget {

void VerticalSliderColorEdit(const char label[], std::array<float, 3>& color) {
  ImVec2 slider_size{10.0f, 60.0f};

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0, 4.0f));

  {
    ImGui::PushID(label);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.0f, 0.0f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

    ImGui::VSliderFloat("##r", slider_size, &color.at(0), 0.0f, 1.0f, "");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("R: %.2f", color.at(0));

    ImGui::PopStyleColor(5);
    ImGui::PopID();
  }

  ImGui::SameLine();

  {
    ImGui::PushID(label);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.0f, 1.0f, 0.0f, 0.2f).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(0.0f, 1.0f, 0.0f, 0.4f).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(0.0f, 1.0f, 0.0f, 0.6f).Value);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor(0.0f, 1.0f, 0.0f, 0.8f).Value);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImColor(0.0f, 1.0f, 0.0f, 1.0f).Value);

    ImGui::VSliderFloat("##g", slider_size, &color.at(1), 0.0f, 1.0f, "");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("G: %.2f", color.at(1));

    ImGui::PopStyleColor(5);
    ImGui::PopID();
  }

  ImGui::SameLine();

  {
    ImGui::PushID(label);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.0f, 0.0f, 1.0f, 0.2f).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(0.0f, 0.0f, 1.0f, 0.4f).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(0.0f, 0.0f, 1.0f, 0.6f).Value);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor(0.0f, 0.0f, 1.0f, 0.8f).Value);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImColor(0.0f, 0.0f, 1.0f, 1.0f).Value);

    ImGui::VSliderFloat("##b", slider_size, &color.at(2), 0.0f, 1.0f, "");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("B: %.2f", color.at(2));

    ImGui::PopStyleColor(5);
    ImGui::PopID();
  }

  ImGui::PopStyleVar();

  ImGui::SameLine();

  ImVec4 button_color{color.at(0), color.at(1), color.at(2), 1.0f};
  ImVec2 button_size{60.0f, 60.0f};
  ImGuiColorEditFlags button_flags =
      ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop;
  ImGui::ColorButton(label, button_color, button_flags, button_size);
  if (ImGui::IsItemActive() || ImGui::IsItemHovered())
    ImGui::SetTooltip("%s", label);
}

}  // namespace cheers::widget
