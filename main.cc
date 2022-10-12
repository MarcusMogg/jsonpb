#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>

#include "hello.pb.h"
#include "src/jsonpb.h"

int main() {
  hello::TestHello test;
  test.set_world("world");
  nlohmann::json json;
  jsonpb::ConvertPbToJson(test, json);
  std::cout << json.dump(4);
  return 0;
}
