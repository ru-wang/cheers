set(CHEERS_PROTO_MSGS
  ${CMAKE_CURRENT_LIST_DIR}/image.proto
  ${CMAKE_CURRENT_LIST_DIR}/misc.proto
  ${CMAKE_CURRENT_LIST_DIR}/model.proto
  ${CMAKE_CURRENT_LIST_DIR}/path.proto)

CheersPackage()

CheersAddProto(PB image MSGS image.proto)
CheersAddProto(PB misc  MSGS misc.proto)
CheersAddProto(PB model MSGS model.proto)
CheersAddProto(PB path  MSGS path.proto)
