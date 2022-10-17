#include <gtest/gtest.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "src/jsonpb.h"
#include "test.pb.h"
#include "test_comm.h"

TEST(PbToJson, BaseMessage) {
  const auto test_msg = GetTestMessage();

  nlohmann::json json;
  jsonpb::ConvertPbToJson(test_msg, json);

  nlohmann::json expected = GetTestJson();

  const auto diff = nlohmann::json::diff(expected, json);
  if (!diff.empty()) {
    std::cout << diff.dump(4);
  }
  EXPECT_TRUE(diff.empty());
}

TEST(PbToJson, BaseMessageWithIgnore) {
  const auto test_msg = GetTestMessage(true);

  nlohmann::json json;
  jsonpb::ConvertPbToJson(test_msg, json);

  nlohmann::json expected = GetTestJson();

  const auto diff = nlohmann::json::diff(expected, json);
  if (!diff.empty()) {
    std::cout << diff.dump(4);
  }
  EXPECT_TRUE(diff.empty());
}

TEST(PbToJson, AliasMessage) {
  const auto test_msg = GetTestAlias();

  nlohmann::json json;
  jsonpb::ConvertPbToJson(test_msg, json);

  nlohmann::json expected = GetAliasJson();

  const auto diff = nlohmann::json::diff(expected, json);
  if (!diff.empty()) {
    std::cout << diff.dump(4);
  }
  EXPECT_TRUE(diff.empty());
}

TEST(PbToJson, AliasMessageWithIgnore) {
  const auto test_msg = GetTestAlias(true);

  nlohmann::json json;
  jsonpb::ConvertPbToJson(test_msg, json);

  nlohmann::json expected = GetAliasJson();

  const auto diff = nlohmann::json::diff(expected, json);
  if (!diff.empty()) {
    std::cout << diff.dump(4);
  }
  EXPECT_TRUE(diff.empty());
}
