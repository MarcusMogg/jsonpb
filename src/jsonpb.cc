#include "jsonpb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <stdint.h>

#include <functional>
#include <string>

#include "protobuf_options.pb.h"

namespace jsonpb {

namespace detail {

using RepeatedToJsonSetter = std::function<bool(const int, nlohmann::json&)>;
using RepeatedToPbSetter = std::function<bool(const int, google::protobuf::Message&)>;

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

void UpdateEnumFieldFromJson(
    const nlohmann::json& json,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    google::protobuf::Message& message) {
  const auto& enum_desc = *field.enum_type();
  int cnt = enum_desc.value_count();

  for (int i = 0; i < cnt; i++) {
    const auto enum_item = enum_desc.value(i);
    if (option.enum_uint) {
      if (json.is_number_unsigned() && enum_item->number() == json.get<uint32_t>()) {
        reflection.SetEnum(&message, &field, enum_item);
        break;
      }
    } else {
      if (json.is_string() && enum_item->name() == json.get<std::string>()) {
        reflection.SetEnum(&message, &field, enum_item);
        break;
      }
    }
  }
}

bool UpdateSingleFiledFromJson(
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    const nlohmann::json& json,
    google::protobuf::Message& message) {
  const std::string field_name = GetFieldName(field);
  bool res = true;

  if (!json.contains(field_name)) {
    return true;
  }
  const auto& json_value = json[field_name];

  if (json_value.is_null()) {
    return true;
  }
  switch (field.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD, VALUETYPE, VARTYPE)                   \
  case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##CPPTYPE:      \
    if (json_value.is_##VALUETYPE()) {                                     \
      reflection.Set##METHOD(&message, &field, json_value.get<VARTYPE>()); \
    }                                                                      \
    break;
    HANDLE_TYPE(INT32, Int32, number_integer, int32_t);
    HANDLE_TYPE(INT64, Int64, number_integer, int64_t);
    HANDLE_TYPE(UINT32, UInt32, number_unsigned, uint32_t);
    HANDLE_TYPE(UINT64, UInt64, number_unsigned, uint64_t);
    HANDLE_TYPE(FLOAT, Float, number_float, float);
    HANDLE_TYPE(DOUBLE, Double, number_float, double);
    HANDLE_TYPE(BOOL, Bool, boolean, bool);
    HANDLE_TYPE(STRING, String, string, std::string);
#undef HANDLE_TYPE
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      UpdateEnumFieldFromJson(json_value, reflection, field, option, message);
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      if (json_value.is_object()) {
        res = ConvertJsonToPb(json[field_name], *reflection.MutableMessage(&message, &field));
      }

      break;
  }
  return res;
}

bool UpdateRepeatedFieldFromJson(
    const nlohmann::json& json,
    const RepeatedToPbSetter setter,
    google::protobuf::Message& message) {
  const int cnt = json.size();
  for (int i = 0; i < cnt; ++i) {
    if (!setter(i, message)) {
      return false;
    }
  }
  return true;
}

bool UpdateRepeatedFieldFromJson(
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    const MessageOptions& option,
    const nlohmann::json& json,
    google::protobuf::Message& message) {
  const std::string field_name = GetFieldName(field);
  bool res = true;

  if (!json.contains(field_name)) {
    return true;
  }
  const auto& json_value = json[field_name];

  if (json_value.is_null() || !json_value.is_array()) {
    return true;
  }

  switch (field.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD, VALUETYPE, VARTYPE)                           \
  case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##CPPTYPE:              \
    UpdateRepeatedFieldFromJson(                                                   \
        json_value,                                                                \
        [&](const int i, google::protobuf::Message& msg) {                         \
          if (json_value.at(i).is_##VALUETYPE()) {                                 \
            reflection.Add##METHOD(&msg, &field, json_value.at(i).get<VARTYPE>()); \
          }                                                                        \
          return true;                                                             \
        },                                                                         \
        message);                                                                  \
    break;
    HANDLE_TYPE(INT32, Int32, number_integer, int32_t);
    HANDLE_TYPE(INT64, Int64, number_integer, int64_t);
    HANDLE_TYPE(UINT32, UInt32, number_unsigned, uint32_t);
    HANDLE_TYPE(UINT64, UInt64, number_unsigned, uint64_t);
    HANDLE_TYPE(FLOAT, Float, number_float, float);
    HANDLE_TYPE(DOUBLE, Double, number_float, double);
    HANDLE_TYPE(BOOL, Bool, boolean, bool);
    HANDLE_TYPE(STRING, String, string, std::string);
#undef HANDLE_TYPE
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      UpdateRepeatedFieldFromJson(
          json_value,
          [&](const int i, google::protobuf::Message& msg) {
            const auto& enum_desc = *field.enum_type();
            int cnt = enum_desc.value_count();
            const auto& json_value_item = json_value.at(i);

            for (int j = 0; j < cnt; j++) {
              const auto enum_item = enum_desc.value(j);
              if (option.enum_uint) {
                if (json_value_item.is_number_unsigned() &&
                    enum_item->number() == json_value_item.get<uint32_t>()) {
                  reflection.AddEnum(&message, &field, enum_item);
                  break;
                }
              } else {
                if (json_value_item.is_string() &&
                    enum_item->name() == json_value_item.get<std::string>()) {
                  reflection.AddEnum(&message, &field, enum_item);
                  break;
                }
              }
            }
            return true;
          },
          message);
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      UpdateRepeatedFieldFromJson(
          json_value,
          [&](const int i, google::protobuf::Message& msg) {
            const auto& json_value_item = json_value.at(i);
            if (json_value_item.is_object()) {
              res = ConvertJsonToPb(json_value_item, *reflection.AddMessage(&message, &field));
            }
            return true;
          },
          message);
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
      res = detail::UpdateRepeatedFieldFromJson(ref, field, option, json, message);
    } else {
      res = detail::UpdateSingleFiledFromJson(ref, field, option, json, message);
    }
    if (!res) {
      return false;
    }
  }
  return true;
}

}  // namespace jsonpb