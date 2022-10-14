find_package(GTest CONFIG REQUIRED)

enable_testing()

file(GLOB test_files test_*.cc)

message("test_files ${test_files}")

add_executable(
  jsonpb_test
  ${test_files}
)


target_compile_features(jsonpb_test PRIVATE cxx_std_17)

target_link_libraries(jsonpb_test PRIVATE 
    libjsonpb
    GTest::gmock 
    GTest::gtest 
    GTest::gmock_main 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(jsonpb_test)
