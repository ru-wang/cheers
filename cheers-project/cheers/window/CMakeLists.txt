bottle_package()

bottle_library(
  NAME arcball_camera
  SRCS arcball_camera.cpp
  HDRS arcball_camera.hpp
  DEPS //cheers/utils:window_ui_state
       @glm
)

bottle_library(
  NAME window
  SRCS window.cpp
       callback.cpp
  HDRS window.hpp
       callback.hpp
  DEPS :arcball_camera
       //cheers/layer:layer
       //cheers/utils:context
       //cheers/utils:im_renderer
       //cheers/utils:window_ui_state
       @glm
       @opengl
)
