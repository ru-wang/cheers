bottle_package()

bottle_library(
  NAME layer
  HDRS layer.hpp
)

bottle_library(
  NAME model_layer
  SRCS model_layer.cpp
  HDRS model_layer.hpp
  DEPS :layer
       //cheers/utils:gl_object
       //cheers/utils:gl_program_wrapper
       //cheers/utils:throw_and_check
       //cheers/proto
       //cheers/resource
       @opengl
)

bottle_library(
  NAME path_layer
  SRCS path_layer.cpp
  HDRS path_layer.hpp
  DEPS :layer
       :utils
       //cheers/utils:gl_object
       //cheers/utils:gl_program_wrapper
       //cheers/utils:throw_and_check
       //cheers/proto
       //cheers/resource
       @glm
       @opengl
)

bottle_library(
  NAME rgb_image_layer
  SRCS rgb_image_layer.cpp
  HDRS rgb_image_layer.hpp
  DEPS :layer
       //cheers/utils:gl_object
       //cheers/proto
       @imgui
       @opengl
)

bottle_library(
  NAME scene_layer
  SRCS scene_layer.cpp
  HDRS scene_layer.hpp
  DEPS :layer
       //cheers/utils:gl_object
       //cheers/utils:gl_program_wrapper
       //cheers/resource
       @imgui
       @opengl
)

bottle_library(
  NAME utils
  HDRS utils.hpp
  DEPS @glm
)
