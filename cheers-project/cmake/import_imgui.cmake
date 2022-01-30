FetchContent_Declare(ocornut.imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        origin/docking
  GIT_SHALLOW    TRUE)

FetchContent_GetProperties(ocornut.imgui)

if(NOT ocornut.imgui_POPULATED)
  FetchContent_Populate(ocornut.imgui)
  add_library(ocornut.imgui SHARED)
  target_sources(ocornut.imgui
    PRIVATE
    ${ocornut.imgui_SOURCE_DIR}/imgui.cpp
    ${ocornut.imgui_SOURCE_DIR}/imgui_demo.cpp
    ${ocornut.imgui_SOURCE_DIR}/imgui_draw.cpp
    ${ocornut.imgui_SOURCE_DIR}/imgui_internal.h
    ${ocornut.imgui_SOURCE_DIR}/imgui_tables.cpp
    ${ocornut.imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${ocornut.imgui_SOURCE_DIR}/imstb_rectpack.h
    ${ocornut.imgui_SOURCE_DIR}/imstb_textedit.h
    ${ocornut.imgui_SOURCE_DIR}/imstb_truetype.h
    PUBLIC
    ${ocornut.imgui_SOURCE_DIR}/imconfig.h
    ${ocornut.imgui_SOURCE_DIR}/imgui.h)
  target_include_directories(ocornut.imgui
    PUBLIC ${ocornut.imgui_SOURCE_DIR})
endif()

bottle_library(
  NAME imgui
  DEPS ocornut.imgui
)
