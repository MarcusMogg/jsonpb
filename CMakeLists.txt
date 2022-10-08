cmake_minimum_required (VERSION 3.14)

project ("jsonpb")


# for clangd
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# 将源代码添加到此项目的可执行文件。
add_executable (jsonpb "main.cc")
target_compile_features(jsonpb PRIVATE cxx_std_17)

add_subdirectory("protos")
target_link_libraries(jsonpb PRIVATE jsonpb_messages)

# for *.pb.h
# target_include_directories(jsonpb
#     PUBLIC
#         ${CMAKE_CURRENT_BINARY_DIR}:protos/
# )

add_subdirectory("src")
# target_link_libraries(jsonpb PRIVATE libjsonpb)

# 单测
add_subdirectory("test")