function(CheersExpandDeps EXPANDED_DEPS)
  if(NOT CHEERS_PKG_NAME)
    message(FATAL_ERROR "CHEERS_PKG_NAME not defined or empty")
  endif()

  foreach(DEP IN LISTS ARGN)
    string(REGEX REPLACE "^:(.+)" "${CHEERS_PKG_NAME}.\\1" DEP ${DEP})
    string(REGEX REPLACE "^/(.+)" "${CHEERS_ROOT_PKG_NAME}.\\1" DEP ${DEP})
    string(REGEX REPLACE "^@(.+)" "${CHEERS_ROOT_PKG_NAME}.deps.\\1" DEP ${DEP})
    list(APPEND ${EXPANDED_DEPS} ${DEP})
  endforeach()
  set(${EXPANDED_DEPS} ${${EXPANDED_DEPS}} PARENT_SCOPE)
endfunction()

function(CheersPopulateSrcs POPULATED_SRCS)
  foreach(SRC IN LISTS ARGN)
    list(APPEND ${POPULATED_SRCS} ${CMAKE_CURRENT_LIST_DIR}/${SRC})
  endforeach()
  set(${POPULATED_SRCS} ${${POPULATED_SRCS}} PARENT_SCOPE)
endfunction()

function(CheersGenerateCfgs GENERATED_CFGS)
  foreach(CFG IN LISTS ARGN)
    get_filename_component(CFG_EXT ${CFG} LAST_EXT)
    string(REGEX REPLACE "^(.+)\\${CFG_EXT}$" "\\1" CFG_NAME ${CFG})
    configure_file(
      ${CMAKE_CURRENT_LIST_DIR}/${CFG}
      ${CMAKE_CURRENT_BINARY_DIR}/${CFG_NAME})
    list(APPEND ${GENERATED_CFGS} ${CMAKE_CURRENT_BINARY_DIR}/${CFG_NAME})
  endforeach()
  set(${GENERATED_CFGS} ${${GENERATED_CFGS}} PARENT_SCOPE)
endfunction()

function(CheersCompileMsgs COMPILED_SRCS COMPILED_HDRS)
  CheersExpandDeps(PROTOC "@protoc")

  foreach(MSG IN LISTS ARGN)
    get_filename_component(MSG_EXT ${MSG} LAST_EXT)
    string(REGEX REPLACE "^(.+)\\${MSG_EXT}$" "\\1" MSG_NAME ${MSG})

    add_custom_command(
      OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${MSG_NAME}.pb.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${MSG_NAME}.pb.h
      COMMAND $<TARGET_FILE:${PROTOC}>
              --proto_path=${PROJECT_SOURCE_DIR}
              --cpp_out=${PROJECT_BINARY_DIR}
              ${CMAKE_CURRENT_LIST_DIR}/${MSG}
      DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${MSG})

    list(APPEND ${COMPILED_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/${MSG_NAME}.pb.cc)
    list(APPEND ${COMPILED_HDRS} ${CMAKE_CURRENT_BINARY_DIR}/${MSG_NAME}.pb.h)
  endforeach()
  set(${COMPILED_SRCS} ${${COMPILED_SRCS}} PARENT_SCOPE)
  set(${COMPILED_HDRS} ${${COMPILED_HDRS}} PARENT_SCOPE)
endfunction()
