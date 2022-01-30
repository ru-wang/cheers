bottle_package()

bottle_library(
  NAME context
  SRCS context.cpp
  HDRS context.hpp
  DEPS :throw_and_check
       @opengl
)

bottle_library(
  NAME gl_backup_state
  SRCS gl_backup_state.cpp
  HDRS gl_backup_state.hpp
  DEPS @opengl
)

bottle_library(
  NAME gl_object
  SRCS gl_object.cpp
  HDRS gl_object.hpp
  DEPS :throw_and_check
       @opengl
)

bottle_library(
  NAME gl_program_wrapper
  SRCS gl_program_wrapper.cpp
  HDRS gl_program_wrapper.hpp
  DEPS :gl_object
       @opengl
)

bottle_library(
  NAME im_renderer
  SRCS im_renderer.cpp
  HDRS im_renderer.hpp
  DEPS :gl_backup_state
       :gl_program_wrapper
       :throw_and_check
       :window_ui_state
       //cheers/shader
       @glm
       @imgui
       @opengl
)

bottle_library(
  NAME throw_and_check
  HDRS throw_and_check.hpp
  DEPS @fmt
)

bottle_library(
  NAME window_ui_state
  HDRS window_ui_state.hpp
  DEPS @glm
       @opengl
)
