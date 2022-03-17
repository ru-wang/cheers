FetchContent_Declare(ru-wang.bottle
  GIT_REPOSITORY https://github.com/ru-wang/bottle.git
  GIT_TAG        v0.1.0
  GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(ru-wang.bottle)

list(APPEND CMAKE_MODULE_PATH ${ru-wang.bottle_SOURCE_DIR})
include(bottle_target)
