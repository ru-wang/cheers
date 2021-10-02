CheersPackage()

CheersAddTarget(
  LIB  layer
  HDRS layer.hpp
)

CheersAddTarget(
  LIB  model_layer
  SRCS model_layer.cpp
  HDRS model_layer.hpp
  DEPS :layer
       /utils.gl_object
       /utils.gl_program_wrapper
       /utils.throw_and_check
       /proto
       /shader
       @opengl
)

CheersAddTarget(
  LIB  path_layer
  SRCS path_layer.cpp
  HDRS path_layer.hpp
  DEPS :layer
       :utils
       /utils.gl_object
       /utils.gl_program_wrapper
       /utils.throw_and_check
       /proto
       /shader
       @glm
       @opengl
)

CheersAddTarget(
  LIB  rgb_image_layer
  SRCS rgb_image_layer.cpp
  HDRS rgb_image_layer.hpp
  DEPS :layer
       /utils.gl_object
       /proto
       @imgui
       @opengl
)

CheersAddTarget(
  LIB  scene_layer
  SRCS scene_layer.cpp
  HDRS scene_layer.hpp
  DEPS :layer
       :widget
       /utils.gl_object
       /utils.gl_program_wrapper
       /shader
       @imgui
       @opengl
)

CheersAddTarget(
  LIB  utils
  HDRS utils.hpp
  DEPS @glm
)

CheersAddTarget(
  LIB  widget
  SRCS widget.cpp
  HDRS widget.hpp
  DEPS @imgui
)
