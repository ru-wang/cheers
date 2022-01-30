FetchContent_Declare(ru-wang.bottle
  GIT_REPOSITORY git@github.com:ru-wang/bottle.git
  GIT_TAG        origin/master
  GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(ru-wang.bottle)

list(APPEND CMAKE_MODULE_PATH ${ru-wang.bottle_SOURCE_DIR})
include(bottle_target)
