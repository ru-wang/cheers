FetchContent_Declare(fmtlib.fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        6.2.1
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(fmtlib.fmt)

CheersAddTarget(
  LIB  fmt
  DEPS fmt::fmt
)
