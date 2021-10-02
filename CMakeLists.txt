cmake_minimum_required(VERSION 3.14)
project(cheers-superproject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to: " ${CMAKE_BUILD_TYPE})
  set(CMAKE_BUILD_TYPE Release)
endif()

find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Found ccache: " ${CCACHE})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif()

option(CHEERS_BUILD_EXAMPLE "Build cheers demo" ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cheers-project)

if (CHEERS_BUILD_EXAMPLE)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cheers-example)
endif()
