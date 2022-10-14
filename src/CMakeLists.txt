file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE CC_FILES "*.cc" "*.cpp")

add_library (libjsonpb STATIC ${HEADER_FILES} ${CC_FILES})

target_compile_features(libjsonpb PRIVATE cxx_std_17)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(libjsonpb PUBLIC 
    nlohmann_json::nlohmann_json
    jsonpb_messages
)

target_include_directories(libjsonpb
    PUBLIC
        ${PROJECT_SOURCE_DIR}/
)
