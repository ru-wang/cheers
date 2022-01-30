find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

bottle_library(
  NAME opengl
  DEPS OpenGL::OpenGL
       GLEW::GLEW
       glfw
)
