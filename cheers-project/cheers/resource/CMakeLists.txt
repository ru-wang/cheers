set(CHEERS_FONT_PATH ${CMAKE_CURRENT_LIST_DIR}/font)

set(CHEERS_FONT_TTF_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/font/Cousine-Regular.ttf
  ${CMAKE_CURRENT_LIST_DIR}/font/DroidSans.ttf
  ${CMAKE_CURRENT_LIST_DIR}/font/Karla-Regular.ttf
  ${CMAKE_CURRENT_LIST_DIR}/font/ProggyClean.ttf
  ${CMAKE_CURRENT_LIST_DIR}/font/ProggyTiny.ttf
  ${CMAKE_CURRENT_LIST_DIR}/font/Roboto-Medium.ttf)

set(CHEERS_GLSL_VERSION_NUMBER 460)
set(CHEERS_GLSL_SHADER_PATH ${CMAKE_CURRENT_LIST_DIR}/shader)

set(CHEERS_VERTEX_SHADER_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/shader/axis_shader_460.vert
  ${CMAKE_CURRENT_LIST_DIR}/shader/grid_shader_460.vert
  ${CMAKE_CURRENT_LIST_DIR}/shader/imgui_shader_460.vert
  ${CMAKE_CURRENT_LIST_DIR}/shader/model_shader_460.vert
  ${CMAKE_CURRENT_LIST_DIR}/shader/path_shader_460.vert)

set(CHEERS_FRAGMENT_SHADER_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/shader/axis_shader_460.frag
  ${CMAKE_CURRENT_LIST_DIR}/shader/grid_shader_460.frag
  ${CMAKE_CURRENT_LIST_DIR}/shader/imgui_shader_460.frag
  ${CMAKE_CURRENT_LIST_DIR}/shader/model_shader_460.frag
  ${CMAKE_CURRENT_LIST_DIR}/shader/path_shader_460.frag)

bottle_package()

bottle_library(
  NAME font_absolute_path
  CFGS font_absolute_path.hpp.in
)

bottle_library(
  NAME shader_absolute_path
  CFGS shader_absolute_path.hpp.in
)
