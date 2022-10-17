#pragma once

#include <nlohmann/json.hpp>

#include "test.pb.h"

constexpr char kTestMessageJson[] = R"({
    "test_int32_singular": 1,
    "test_int32_multiple": [
        1,
        2,
        3,
        4
    ],
    "test_int64_singular": 1,
    "test_int64_multiple": [
        1,
        2,
        3,
        4
    ],
    "test_uint32_singular": 1,
    "test_uint32_multiple": [
        1,
        2,
        3,
        4
    ],
    "test_uint64_singular": 1,
    "test_uint64_multiple": [
        1,
        2,
        3,
        4
    ],
    "test_double_singular": 1.0,
    "test_double_multiple": [
        1.0,
        2,
        1000000000
    ],
    "test_float_singular": 1.0,
    "test_float_multiple": [
        1.0,
        2,
        1000000000
    ],
    "test_bool_singular": true,
    "test_bool_multiple": [
        true,
        false,
        true
    ],
    "test_enum_singular": "TEST_B",
    "test_enum_multiple": [
        "TEST_B",
        "TEST_A"
    ],
    "test_string_singular": "TEST_B",
    "test_string_multiple": [
        "TEST_B",
        "TEST_A"
    ],
    "test_message_singular": {
        "NestedString": "12133",
        "NestedBool": true,
        "nested_enum": "TEST_B",
        "nested_int32": 1
    },
    "test_message_multiple": [
        {
            "NestedString": "1",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 1
        },
        {
            "NestedString": "2",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 2
        },
        {
            "NestedString": "3",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 3
        }
    ]
})";

constexpr char kTestAliasJson[] = R"(
    {
    "TestInt32Singular": 1,
    "TestInt32Multiple": [
        1,
        2,
        3,
        4
    ],
    "TestInt64Singular": 1,
    "TestInt64Multiple": [
        1,
        2,
        3,
        4
    ],
    "TestUInt32Singular": 1,
    "TestUInt32Multiple": [
        1,
        2,
        3,
        4
    ],
    "TestUInt64Singular": 1,
    "TestUInt64Multiple": [
        1,
        2,
        3,
        4
    ],
    "TestDoubleSingular": 1.0,
    "TestDoubleMultiple": [
        1.0,
        2,
        1000000000
    ],
    "TestFloatSingular": 1.0,
    "TestFloatMultiple": [
        1.0,
        2,
        1000000000
    ],
    "TestBoolSingular": true,
    "TestBoolMultiple": [
        true,
        false,
        true
    ],
    "TestEnumSingular": 1,
    "TestEnumMultiple": [
        1,
        0
    ],
    "TestStringSingular": "TEST_B",
    "TestStringMultiple": [
        "TEST_B",
        "TEST_A"
    ],
    "TestMessageSingular": {
        "NestedString": "12133",
        "NestedBool": true,
        "nested_enum": "TEST_B",
        "nested_int32": 1
    },
    "TestMessageMultiple": [
        {
            "NestedString": "1",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 1
        },
        {
            "NestedString": "2",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 2
        },
        {
            "NestedString": "3",
            "NestedBool": true,
            "nested_enum": "TEST_B",
            "nested_int32": 3
        }
    ]
}
)";

static nlohmann::json GetTestJson(bool with_ignore = false) {
  nlohmann::json res = nlohmann::json::parse(kTestMessageJson);
  if (with_ignore) {
    res["test_int32_singular_ignore"] = 1;
    res["test_int32_multiple_ignore"].emplace_back(1);
    res["test_int32_multiple_ignore"].emplace_back(2);
  }
  return res;
}

static nlohmann::json GetAliasJson(bool with_ignore = false) {
  nlohmann::json res = nlohmann::json::parse(kTestAliasJson);
  if (with_ignore) {
    res["TestInt32SingularIgnore"] = 1;
    res["TestInt32MultipleIgnore"].emplace_back(1);
    res["TestInt32MultipleIgnore"].emplace_back(2);
  }
  return res;
}

static test_jsonpb::TestMessage GetTestMessage(bool with_ignore = false) {
  test_jsonpb::TestMessage res;
  res.set_test_int32_singular(1);
  res.mutable_test_int32_multiple()->Add(1);
  res.mutable_test_int32_multiple()->Add(2);
  res.mutable_test_int32_multiple()->Add(3);
  res.mutable_test_int32_multiple()->Add(4);
  res.set_test_int64_singular(1);
  res.mutable_test_int64_multiple()->Add(1);
  res.mutable_test_int64_multiple()->Add(2);
  res.mutable_test_int64_multiple()->Add(3);
  res.mutable_test_int64_multiple()->Add(4);
  res.set_test_uint32_singular(1);
  res.mutable_test_uint32_multiple()->Add(1);
  res.mutable_test_uint32_multiple()->Add(2);
  res.mutable_test_uint32_multiple()->Add(3);
  res.mutable_test_uint32_multiple()->Add(4);
  res.set_test_uint64_singular(1);
  res.mutable_test_uint64_multiple()->Add(1);
  res.mutable_test_uint64_multiple()->Add(2);
  res.mutable_test_uint64_multiple()->Add(3);
  res.mutable_test_uint64_multiple()->Add(4);
  res.set_test_double_singular(1.0);
  res.mutable_test_double_multiple()->Add(1.0);
  res.mutable_test_double_multiple()->Add(2.0);
  res.mutable_test_double_multiple()->Add(1000000000);
  res.set_test_float_singular(1.0);
  res.mutable_test_float_multiple()->Add(1.0);
  res.mutable_test_float_multiple()->Add(2.0);
  res.mutable_test_float_multiple()->Add(1000000000);

  res.set_test_bool_singular(true);
  res.mutable_test_bool_multiple()->Add(true);
  res.mutable_test_bool_multiple()->Add(false);
  res.mutable_test_bool_multiple()->Add(true);

  res.set_test_enum_singular(test_jsonpb::TEST_B);
  res.mutable_test_enum_multiple()->Add(test_jsonpb::TEST_B);
  res.mutable_test_enum_multiple()->Add(test_jsonpb::TEST_A);

  res.set_test_string_singular("TEST_B");
  res.mutable_test_string_multiple()->Add("TEST_B");
  res.mutable_test_string_multiple()->Add("TEST_A");

  test_jsonpb::NestedMessage nest;
  nest.set_nested_string("12133");
  nest.set_nested_bool(true);
  nest.set_nested_enum(test_jsonpb::TEST_B);
  nest.set_nested_int32(1);
  *res.mutable_test_message_singular() = nest;

  nest.set_nested_string("1");
  *res.mutable_test_message_multiple()->Add() = nest;

  nest.set_nested_string("2");
  nest.set_nested_int32(2);
  *res.mutable_test_message_multiple()->Add() = nest;

  nest.set_nested_string("3");
  nest.set_nested_int32(3);
  *res.mutable_test_message_multiple()->Add() = nest;

  if (with_ignore) {
    res.set_test_int32_singular_ignore(1);
    res.mutable_test_int32_multiple_ignore()->Add(1);
    res.mutable_test_int32_multiple_ignore()->Add(2);
  }
  return res;
}

static test_jsonpb::TestAlias GetTestAlias(bool with_ignore = false) {
  test_jsonpb::TestAlias res;
  res.set_test_int32_singular(1);
  res.mutable_test_int32_multiple()->Add(1);
  res.mutable_test_int32_multiple()->Add(2);
  res.mutable_test_int32_multiple()->Add(3);
  res.mutable_test_int32_multiple()->Add(4);
  res.set_test_int64_singular(1);
  res.mutable_test_int64_multiple()->Add(1);
  res.mutable_test_int64_multiple()->Add(2);
  res.mutable_test_int64_multiple()->Add(3);
  res.mutable_test_int64_multiple()->Add(4);
  res.set_test_uint32_singular(1);
  res.mutable_test_uint32_multiple()->Add(1);
  res.mutable_test_uint32_multiple()->Add(2);
  res.mutable_test_uint32_multiple()->Add(3);
  res.mutable_test_uint32_multiple()->Add(4);
  res.set_test_uint64_singular(1);
  res.mutable_test_uint64_multiple()->Add(1);
  res.mutable_test_uint64_multiple()->Add(2);
  res.mutable_test_uint64_multiple()->Add(3);
  res.mutable_test_uint64_multiple()->Add(4);
  res.set_test_double_singular(1.0);
  res.mutable_test_double_multiple()->Add(1.0);
  res.mutable_test_double_multiple()->Add(2.0);
  res.mutable_test_double_multiple()->Add(1000000000);
  res.set_test_float_singular(1.0);
  res.mutable_test_float_multiple()->Add(1.0);
  res.mutable_test_float_multiple()->Add(2.0);
  res.mutable_test_float_multiple()->Add(1000000000);

  res.set_test_bool_singular(true);
  res.mutable_test_bool_multiple()->Add(true);
  res.mutable_test_bool_multiple()->Add(false);
  res.mutable_test_bool_multiple()->Add(true);

  res.set_test_enum_singular(test_jsonpb::TEST_B);
  res.mutable_test_enum_multiple()->Add(test_jsonpb::TEST_B);
  res.mutable_test_enum_multiple()->Add(test_jsonpb::TEST_A);

  res.set_test_string_singular("TEST_B");
  res.mutable_test_string_multiple()->Add("TEST_B");
  res.mutable_test_string_multiple()->Add("TEST_A");

  test_jsonpb::NestedMessage nest;
  nest.set_nested_string("12133");
  nest.set_nested_bool(true);
  nest.set_nested_enum(test_jsonpb::TEST_B);
  nest.set_nested_int32(1);
  *res.mutable_test_message_singular() = nest;

  nest.set_nested_string("1");
  *res.mutable_test_message_multiple()->Add() = nest;

  nest.set_nested_string("2");
  nest.set_nested_int32(2);
  *res.mutable_test_message_multiple()->Add() = nest;

  nest.set_nested_string("3");
  nest.set_nested_int32(3);
  *res.mutable_test_message_multiple()->Add() = nest;

  if (with_ignore) {
    res.set_test_int32_singular_ignore(1);
    res.mutable_test_int32_multiple_ignore()->Add(1);
    res.mutable_test_int32_multiple_ignore()->Add(2);
  }
  return res;
}