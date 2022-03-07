FetchContent_Declare(ocornut.imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        origin/docking
  GIT_SHALLOW    TRUE)

FetchContent_Declare(epezent.implot
  GIT_REPOSITORY https://github.com/epezent/implot.git
  GIT_TAG        origin/master
  GIT_SHALLOW    TRUE)

FetchContent_GetProperties(ocornut.imgui)
FetchContent_GetProperties(epezent.implot)

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
  target_include_directories(ocornut.imgui PUBLIC ${ocornut.imgui_SOURCE_DIR})
endif()

if(NOT epezent.implot_POPULATED)
  FetchContent_Populate(epezent.implot)
  add_library(epezent.implot SHARED)
  target_sources(epezent.implot
    PRIVATE
    ${epezent.implot_SOURCE_DIR}/implot.cpp
    ${epezent.implot_SOURCE_DIR}/implot_demo.cpp
    ${epezent.implot_SOURCE_DIR}/implot_items.cpp
    PUBLIC
    ${epezent.implot_SOURCE_DIR}/implot.h
    ${epezent.implot_SOURCE_DIR}/implot_internal.h)
  target_link_libraries(epezent.implot PUBLIC ocornut.imgui)
  target_include_directories(epezent.implot PUBLIC ${epezent.implot_SOURCE_DIR})
endif()

bottle_library(
  NAME imgui
  DEPS ocornut.imgui
)

bottle_library(
  NAME implot
  DEPS epezent.implot
)
