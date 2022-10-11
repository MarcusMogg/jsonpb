#pragma once

#include "google/protobuf/message.h"
#include "nlohmann/json.hpp"

namespace jsonpb {
bool ConvertPbToJson(const google::protobuf::Message& message, nlohmann::json& json);
bool ConvertJsonToPb(const nlohmann::json& json, google::protobuf::Message& message);
}  // namespace jsonpb