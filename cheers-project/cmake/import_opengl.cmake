find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

CheersAddTarget(
  LIB  opengl
  DEPS OpenGL::OpenGL
       GLEW::GLEW
       glfw
)
