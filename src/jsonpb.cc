#include "jsonpb.h"

#include <google/protobuf/descriptor.h>

#include <functional>
#include <string>

#include "protobuf_options.pb.h"

namespace jsonpb {

namespace detail {

struct MessageOptions {
  bool enum_uint;
};

std::string GetFieldName(const google::protobuf::FieldDescriptor& field) {
  const std::string& name = field.options().GetExtension(protobuf_options::alias_name);
  if (name.empty()) {
    return field.name();
  }
  return field.options().GetExtension(protobuf_options::alias_name);
}

void ConvertEnumFieldToJson(
    const google::protobuf::Message& message,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    nlohmann::json& json) {
  const auto& enum_desc = *reflection.GetEnum(message, &field);
  if (option.enum_uint) {
    json = enum_desc.number();
  } else {
    json = enum_desc.name();
  }
}

bool ConvertSingleFiledToJson(
    const google::protobuf::Message& message,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    nlohmann::json& json) {
  const std::string field_name = GetFieldName(field);
  bool res = true;

  switch (field.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                  \
  case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##CPPTYPE: \
    json[field_name] = reflection.Get##METHOD(message, &field);       \
    break;
    HANDLE_TYPE(INT32, Int32);
    HANDLE_TYPE(INT64, Int64);
    HANDLE_TYPE(UINT32, UInt32);
    HANDLE_TYPE(UINT64, UInt64);
    HANDLE_TYPE(FLOAT, Float);
    HANDLE_TYPE(DOUBLE, Double);
    HANDLE_TYPE(BOOL, Bool);
    HANDLE_TYPE(STRING, String);
#undef HANDLE_TYPE
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      ConvertEnumFieldToJson(message, reflection, field, option, json[field_name]);
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      res = ConvertPbToJson(reflection.GetMessage(message, &field), json[field_name]);
      break;
  }
  return res;
}

using RepeatedToJsonSetter = std::function<bool(const int, nlohmann::json&)>;

bool ConvertRepeatedFieldToJson(
    const google::protobuf::Message& message,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const RepeatedToJsonSetter setter,
    nlohmann::json& json) {
  const int cnt = reflection.FieldSize(message, &field);

  for (int i = 0; i < cnt; ++i) {
    if (!setter(i, json)) {
      return false;
    }
  }
  return true;
}
bool ConvertRepeatedFieldToJson(
    const google::protobuf::Message& message,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    nlohmann::json& json) {
  const std::string field_name = GetFieldName(field);
  bool res = true;

  switch (field.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                                   \
  case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##CPPTYPE:                  \
    res = ConvertRepeatedFieldToJson(                                                  \
        message,                                                                       \
        reflection,                                                                    \
        field,                                                                         \
        [&](const int i, nlohmann::json& json_value) {                                 \
          json_value.emplace_back(reflection.GetRepeated##METHOD(message, &field, i)); \
          return true;                                                                 \
        },                                                                             \
        json[field_name]);                                                             \
    break;
    HANDLE_TYPE(INT32, Int32);
    HANDLE_TYPE(INT64, Int64);
    HANDLE_TYPE(UINT32, UInt32);
    HANDLE_TYPE(UINT64, UInt64);
    HANDLE_TYPE(FLOAT, Float);
    HANDLE_TYPE(DOUBLE, Double);
    HANDLE_TYPE(BOOL, Bool);
    HANDLE_TYPE(STRING, String);
#undef HANDLE_TYPE
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      res = ConvertRepeatedFieldToJson(
          message,
          reflection,
          field,
          [&](const int i, nlohmann::json& json_value) {
            const auto& enum_desc = *reflection.GetRepeatedEnum(message, &field, i);
            if (option.enum_uint) {
              json_value.emplace_back(enum_desc.number());
            } else {
              json_value.emplace_back(enum_desc.name());
            }
            return true;
          },
          json[field_name]);
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      res = ConvertRepeatedFieldToJson(
          message,
          reflection,
          field,
          [&](const int i, nlohmann::json& json_value) {
            nlohmann::json j;
            bool res = ConvertPbToJson(reflection.GetRepeatedMessage(message, &field, i), j);
            json_value.emplace_back(j);
            return res;
          },
          json[field_name]);
      break;
  }
  return res;
}
}  // namespace detail

bool ConvertPbToJson(const google::protobuf::Message& message, nlohmann::json& json) {
  const auto& ref = *message.GetReflection();
  const auto& des = *message.GetDescriptor();
  detail::MessageOptions option;
  option.enum_uint = des.options().GetExtension(protobuf_options::enum_uint);
  for (int i = 0; i < des.field_count(); ++i) {
    const auto& field = *des.field(i);
    if (field.options().GetExtension(protobuf_options::ignore)) {
      continue;
    }
    bool res = true;
    if (field.is_repeated()) {
      res = detail::ConvertRepeatedFieldToJson(message, ref, field, option, json);
    } else {
      res = detail::ConvertSingleFiledToJson(message, ref, field, option, json);
    }
    if (!res) {
      return false;
    }
  }
  return true;
}
bool ConvertJsonToPb(const nlohmann::json& json, google::protobuf::Message& message) {
  return true;
}

}  // namespace jsonpb