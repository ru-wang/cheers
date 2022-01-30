set(CHEERS_PROTO_MSGS
  ${CMAKE_CURRENT_LIST_DIR}/image.proto
  ${CMAKE_CURRENT_LIST_DIR}/misc.proto
  ${CMAKE_CURRENT_LIST_DIR}/model.proto
  ${CMAKE_CURRENT_LIST_DIR}/path.proto)

bottle_package()

bottle_proto(NAME image MSGS image.proto)
bottle_proto(NAME misc  MSGS misc.proto)
bottle_proto(NAME model MSGS model.proto)
bottle_proto(NAME path  MSGS path.proto)
