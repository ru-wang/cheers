FetchContent_Declare(protocolbuffers.protobuf
  GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
  GIT_TAG        v3.17.3
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  cmake)

set(protobuf_BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(protocolbuffers.protobuf)

bottle_library(
  NAME protobuf
  DEPS protobuf::libprotobuf
)

bottle_binary(
  NAME protoc
  DEPS protobuf::protoc
)
