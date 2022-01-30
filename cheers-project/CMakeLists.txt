cmake_minimum_required(VERSION 3.14)
project(cheers-project LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
add_compile_options(-fdiagnostics-color=always -pedantic -Wall
  $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Debug>:-ggdb3> $<$<CONFIG:Release>:-O3>)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(FetchContent)
include(import_bottle)

bottle_subdir(NAME cheers)
bottle_subdir(NAME depends)
