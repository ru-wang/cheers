FetchContent_Declare(protocolbuffers.protobuf
  GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
  GIT_TAG        v3.17.3
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  cmake)

set(protobuf_BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(protocolbuffers.protobuf)

CheersAddTarget(
  LIB  protobuf
  DEPS protobuf::libprotobuf
  OPTS -w
)

CheersAddTarget(
  BIN  protoc
  DEPS protoc
  OPTS -w
)
