find_package(Protobuf REQUIRED)

file(GLOB_RECURSE PROTO_FILES "*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library (jsonpb_messages STATIC ${PROTO_SRCS} ${PROTO_HDRS})

target_compile_features(jsonpb_messages PRIVATE cxx_std_17)

target_link_libraries(jsonpb_messages PUBLIC ${Protobuf_LIBRARIES})
target_include_directories(jsonpb_messages
    PUBLIC
        ${Protobuf_INCLUDE_DIRS}/
        ${CMAKE_CURRENT_BINARY_DIR}/
)

message("Protobuf_INCLUDE_DIRS ${Protobuf_INCLUDE_DIRS}")
message("CMAKE_CURRENT_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR}")
