#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "hello.pb.h"
#include "src/jsonpb.h"

TEST(PbToJson, Simple) {
  hello::TestHello test;
  test.set_world("world");
  nlohmann::json json;
  jsonpb::ConvertPbToJson(test, json);

  nlohmann::json expected = nlohmann::json::parse(R"(
    {
        "world": "world"
    }  
  )");

  EXPECT_TRUE(nlohmann::json::diff(expected, json).empty());
}

TEST(JsonToPb, Simple) {
  nlohmann::json origin = nlohmann::json::parse(R"(
    {
        "world": "world"
    }  
  )");

  hello::TestHello test;
  jsonpb::ConvertJsonToPb(origin, test);

  hello::TestHello expected;
  expected.set_world("world");

  google::protobuf::util::MessageDifferencer differ;
  EXPECT_TRUE(differ.Compare(expected, test));
}

int main(int argc, char** argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}