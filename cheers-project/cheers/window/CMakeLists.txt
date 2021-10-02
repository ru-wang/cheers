CheersPackage()

CheersAddTarget(
  LIB  arcball_camera
  SRCS arcball_camera.cpp
  HDRS arcball_camera.hpp
  DEPS /utils.window_ui_state
       @glm
)

CheersAddTarget(
  LIB  window
  SRCS window.cpp
       callback.cpp
  HDRS window.hpp
       callback.hpp
  DEPS :arcball_camera
       /layer.layer
       /utils.context
       /utils.im_renderer
       /utils.window_ui_state
       @glm
       @opengl
)
