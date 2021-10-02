set(CHEERS_GLSL_VERSION_NUMBER 410)
set(CHEERS_GLSL_SHADER_PATH ${CMAKE_CURRENT_LIST_DIR})

set(CHEERS_VERTEX_SHADER_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/axis_shader_410.vert
  ${CMAKE_CURRENT_LIST_DIR}/grid_shader_410.vert
  ${CMAKE_CURRENT_LIST_DIR}/imgui_shader_410.vert
  ${CMAKE_CURRENT_LIST_DIR}/model_shader_410.vert
  ${CMAKE_CURRENT_LIST_DIR}/path_shader_410.vert)

set(CHEERS_FRAGMENT_SHADER_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/axis_shader_410.frag
  ${CMAKE_CURRENT_LIST_DIR}/grid_shader_410.frag
  ${CMAKE_CURRENT_LIST_DIR}/imgui_shader_410.frag
  ${CMAKE_CURRENT_LIST_DIR}/model_shader_410.frag
  ${CMAKE_CURRENT_LIST_DIR}/path_shader_410.frag)

CheersPackage()

CheersAddTarget(
  LIB  shader_absolute_path
  CFGS shader_absolute_path.hpp.in
)
