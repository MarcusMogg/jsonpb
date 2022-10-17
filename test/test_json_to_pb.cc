#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "src/jsonpb.h"
#include "test.pb.h"
#include "test_comm.h"

TEST(JsonToPb, BaseMessage) {
  nlohmann::json test_json = GetTestJson();
  const auto expected = GetTestMessage();

  test_jsonpb::TestMessage output;
  jsonpb::ConvertJsonToPb(test_json, output);

  google::protobuf::util::MessageDifferencer differ;
  EXPECT_TRUE(differ.Compare(expected, output));
}

TEST(JsonToPb, BaseMessageWithIgnore) {
  nlohmann::json test_json = GetTestJson(true);
  const auto expected = GetTestMessage();

  test_jsonpb::TestMessage output;
  jsonpb::ConvertJsonToPb(test_json, output);

  google::protobuf::util::MessageDifferencer differ;
  EXPECT_TRUE(differ.Compare(expected, output));
}

TEST(JsonToPb, AliasMessage) {
  nlohmann::json test_json = GetAliasJson();
  const auto expected = GetTestAlias();

  test_jsonpb::TestAlias output;
  jsonpb::ConvertJsonToPb(test_json, output);

  google::protobuf::util::MessageDifferencer differ;
  EXPECT_TRUE(differ.Compare(expected, output));
}

TEST(JsonToPb, AliasMessageWithIgnore) {
  nlohmann::json test_json = GetAliasJson(true);
  const auto expected = GetTestAlias();

  test_jsonpb::TestAlias output;
  jsonpb::ConvertJsonToPb(test_json, output);

  google::protobuf::util::MessageDifferencer differ;
  EXPECT_TRUE(differ.Compare(expected, output));
}
