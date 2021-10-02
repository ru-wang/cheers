include(cheers_target_helper)

function(CheersPackage)
  file(RELATIVE_PATH CHEERS_PKG_NAME ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
  string(REPLACE "/" "." CHEERS_PKG_NAME ${CHEERS_PKG_NAME})
  set(CHEERS_PKG_NAME ${CHEERS_PKG_NAME} PARENT_SCOPE)
  if(NOT CHEERS_ROOT_PKG_NAME)
    set(CHEERS_ROOT_PKG_NAME ${CHEERS_PKG_NAME} PARENT_SCOPE)
  endif()

  add_library(${CHEERS_PKG_NAME} INTERFACE)
  message(VERBOSE "Create package target with name ${CHEERS_PKG_NAME}")
endfunction()

function(CheersTryLinkPackage CHEERS_TGT_NAME)
  if(CHEERS_PKG_NAME AND CHEERS_TGT_NAME)
    target_link_libraries(${CHEERS_PKG_NAME} INTERFACE ${CHEERS_TGT_NAME})
    message(VERBOSE "Link package target with ${CHEERS_TGT_NAME}")
  endif()
endfunction()

function(CheersAddTarget)
  # parse arguments
  cmake_parse_arguments(CHEERS_TGT "" "LIB;BIN" "SRCS;HDRS;CFGS;DEPS;OPTS;DEFS" ${ARGN})

  # check arguments
  if(CHEERS_TGT_LIB AND CHEERS_TGT_BIN)
    # both LIB and BIN defined as non-empty
    message(FATAL_ERROR "Creating target fails: either create a named LIB or a named BIN")
  elseif(NOT CHEERS_TGT_LIB AND NOT CHEERS_TGT_BIN)
    # neither LIB nor BIN defined as non-empty
    message(FATAL_ERROR "Creating target fails: must provide a named LIB or a named BIN")
  elseif(CHEERS_TGT_BIN)
    # BIN defined as non-empty
    if(NOT CHEERS_TGT_SRCS AND NOT CHEERS_TGT_DEPS)
      # neither SRCS or DEPS not defined or empty
      message(FATAL_ERROR "Creating target fails: must provide SRCS or DEPS for ${CHEERS_TGT_BIN}")
    endif()
  endif()

  string(JOIN "." CHEERS_TGT_NAME ${CHEERS_PKG_NAME} ${CHEERS_TGT_LIB} ${CHEERS_TGT_BIN})
  CheersExpandDeps(CHEERS_TGT_EXPANDED_DEPS ${CHEERS_TGT_DEPS})
  CheersPopulateSrcs(CHEERS_TGT_SOURCES ${CHEERS_TGT_SRCS})
  CheersPopulateSrcs(CHEERS_TGT_HEADERS ${CHEERS_TGT_HDRS})
  CheersGenerateCfgs(CHEERS_TGT_GENERATED ${CHEERS_TGT_CFGS})

  if(CHEERS_TGT_LIB)
    if(CHEERS_TGT_SOURCES)
      set(CHEERS_TGT_TRANSCOPE PUBLIC)
      add_library(${CHEERS_TGT_NAME} SHARED)
      target_sources(${CHEERS_TGT_NAME}
        PRIVATE ${CHEERS_TGT_SOURCES}
        PUBLIC  ${CHEERS_TGT_HEADERS} ${CHEERS_TGT_GENERATED})
    else()
      set(CHEERS_TGT_TRANSCOPE INTERFACE)
      add_library(${CHEERS_TGT_NAME} INTERFACE)
      target_sources(${CHEERS_TGT_NAME}
        INTERFACE ${CHEERS_TGT_HEADERS} ${CHEERS_TGT_GENERATED})
    endif()

    CheersTryLinkPackage(${CHEERS_TGT_NAME})
  else()
    if(CHEERS_TGT_SOURCES)
      set(CHEERS_TGT_TRANSCOPE PRIVATE)
      add_executable(${CHEERS_TGT_NAME})
      target_sources(${CHEERS_TGT_NAME}
        PRIVATE ${CHEERS_TGT_SOURCES} ${CHEERS_TGT_HEADERS} ${CHEERS_TGT_GENERATED})
    else()
      add_executable(${CHEERS_TGT_NAME} ALIAS ${CHEERS_TGT_EXPANDED_DEPS})
    endif()
  endif()

  if(CHEERS_TGT_TRANSCOPE)
    target_compile_features(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} cxx_std_17)
    target_compile_options(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} ${CHEERS_TGT_OPTS})
    target_compile_definitions(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} ${CHEERS_TGT_DEFS})
    target_include_directories(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} ${PROJECT_SOURCE_DIR})
    if(CHEERS_TGT_GENERATED)
      target_include_directories(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} ${PROJECT_BINARY_DIR})
    endif()
    target_link_libraries(${CHEERS_TGT_NAME} ${CHEERS_TGT_TRANSCOPE} ${CHEERS_TGT_EXPANDED_DEPS})
  endif()

  message(VERBOSE "Create target with name ${CHEERS_TGT_NAME}")
endfunction()

function(CheersAddProto)
  # parse arguments
  cmake_parse_arguments(CHEERS_TGT "" "PB" "MSGS" ${ARGN})

  # check arguments
  if(NOT CHEERS_TGT_PB)
    # BIN not defined or empty
    message(FATAL_ERROR "Creating proto target fails: must provide a named PB")
  elseif(NOT CHEERS_TGT_MSGS)
    # BIN defined as non-empty but SRCS not defined or empty
    message(FATAL_ERROR "Creating proto target fails: must provide MSGS for ${CHEERS_TGT_PB}")
  endif()

  string(JOIN "." CHEERS_TGT_NAME ${CHEERS_PKG_NAME} ${CHEERS_TGT_PB})
  CheersExpandDeps(CHEERS_TGT_PROTO "@protobuf")
  CheersCompileMsgs(CHEERS_TGT_SOURCES CHEERS_TGT_HEADERS ${CHEERS_TGT_MSGS})

  add_library(${CHEERS_TGT_NAME} SHARED)
  target_sources(${CHEERS_TGT_NAME} PRIVATE ${CHEERS_TGT_SOURCES})
  target_include_directories(${CHEERS_TGT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
  target_link_libraries(${CHEERS_TGT_NAME} PUBLIC ${CHEERS_TGT_PROTO})

  CheersTryLinkPackage(${CHEERS_TGT_NAME})
  message(VERBOSE "Create proto target with name ${CHEERS_TGT_NAME}")
endfunction()

function(CheersAddSubDir)
  # parse arguments
  cmake_parse_arguments(CHEERS_TGT "" "DIR" "" ${ARGN})

  if(NOT CHEERS_TGT_DIR)
    # DIR not defined or empty
    message(FATAL_ERROR "Adding subdirectory fails: must provide a named DIR")
  endif()

  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${CHEERS_TGT_DIR})

  string(REPLACE "/" "." CHEERS_TGT_DIR ${CHEERS_TGT_DIR})
  string(JOIN "." CHEERS_TGT_NAME ${CHEERS_PKG_NAME} ${CHEERS_TGT_DIR})
  CheersTryLinkPackage(${CHEERS_TGT_NAME})
  message(VERBOSE "Add subdirectory with name ${CHEERS_TGT_NAME}")
endfunction()
