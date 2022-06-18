cmake_minimum_required(VERSION 3.14)
project(cheers-example LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
add_compile_options(-fdiagnostics-color=always -pedantic -Wall
  $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Debug>:-ggdb3> $<$<CONFIG:Release>:-O3>)

find_package(Eigen3 REQUIRED NO_MODULE)

include(FetchContent)
FetchContent_Declare(nothings.stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        origin/master
  GIT_SHALLOW    TRUE)
FetchContent_GetProperties(nothings.stb)
if(NOT nothings.stb_POPULATED)
  FetchContent_Populate(nothings.stb)
  add_library(stb_image INTERFACE)
  target_sources(stb_image
    INTERFACE
    ${nothings.stb_SOURCE_DIR}/stb_image.h
    ${nothings.stb_SOURCE_DIR}/stb_image_write.h
    ${nothings.stb_SOURCE_DIR}/stb_image_resize.h)
  target_include_directories(stb_image
    INTERFACE ${nothings.stb_SOURCE_DIR})
endif()

add_library(demo_layer SHARED)
target_sources(demo_layer
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/demo_layer.cpp
  PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/demo_layer.hpp)
target_link_libraries(demo_layer
  PUBLIC cheers-project::cheers Eigen3::Eigen)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/demo_shader_450.vert
  ${CMAKE_CURRENT_BINARY_DIR}/demo_shader_450.vert
  COPYONLY)
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/demo_shader_450.frag
  ${CMAKE_CURRENT_BINARY_DIR}/demo_shader_450.frag
  COPYONLY)

add_executable(cheers_example_st
  ${CMAKE_CURRENT_LIST_DIR}/cheers_example.cpp
  ${CMAKE_CURRENT_LIST_DIR}/interpolator.hpp
  ${CMAKE_CURRENT_LIST_DIR}/task.cpp
  ${CMAKE_CURRENT_LIST_DIR}/task.hpp)
target_link_libraries(cheers_example_st
  PRIVATE demo_layer cheers-project::cheers Eigen3::Eigen stb_image)

add_executable(cheers_example_mt
  ${CMAKE_CURRENT_LIST_DIR}/cheers_example.cpp
  ${CMAKE_CURRENT_LIST_DIR}/interpolator.hpp
  ${CMAKE_CURRENT_LIST_DIR}/task.cpp
  ${CMAKE_CURRENT_LIST_DIR}/task.hpp)
target_link_libraries(cheers_example_mt
  PRIVATE demo_layer cheers-project::cheers Eigen3::Eigen stb_image)
target_compile_definitions(cheers_example_mt
  PRIVATE CHEERS_EXAMPLE_MULTITHREAD)
