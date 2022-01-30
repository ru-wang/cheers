FetchContent_Declare(g-truc.glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(g-truc.glm)

bottle_library(
  NAME glm
  DEFS GLM_ENABLE_EXPERIMENTAL
  DEPS glm::glm
)
