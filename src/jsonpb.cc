#include "jsonpb.h"

#include <google/protobuf/descriptor.h>

#include <string>

#include "protobuf_options.pb.h"

namespace jsonpb {

namespace detail {

std::string GetFieldName(const google::protobuf::FieldDescriptor& field) {
  const std::string& name = field.options().GetExtension(protobuf_options::alias_name);
  if (name.empty()) {
    return field.name();
  }
  return field.options().GetExtension(protobuf_options::alias_name);
}

bool ConvertSingleFiledToJson(
    const google::protobuf::Message& message,
    const google::protobuf::Reflection& reflection,
    const google::protobuf::FieldDescriptor& field,
    nlohmann::json& json) {
  const std::string field_name = GetFieldName(field);
  switch (field.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                   \
  case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##CPPTYPE:  \
    json.emplace(field_name, reflection.Get##METHOD(message, &field)); \
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
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:  // TYPE_ENUM
      const auto& enum_desc = *reflection.GetEnum(message, &field);
      json.emplace(field_name, reflection.GetInt32(message, &field));
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:  // TYPE_STRING, TYPE_BYTES
      json.emplace(field_name, reflection.GetInt32(message, &field));
      break;
  }
}
bool ConvertRepeatedFiledToJson() {}
}  // namespace detail

bool ConvertPbToJson(const google::protobuf::Message& message, nlohmann::json& json) {
  const auto& ref = *message.GetReflection();
  const auto& des = *message.GetDescriptor();
  for (int i = 0; i < des.field_count(); ++i) {
    const auto& field = *des.field(i);

    if (field.is_repeated()) {
    } else {
    }
  }
  return true;
}
bool ConvertJsonToPb(const nlohmann::json& json, google::protobuf::Message& message) {
  return true;
}

}  // namespace jsonpb