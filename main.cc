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

  hello::TestHello test1;
  jsonpb::ConvertJsonToPb(json, test1);
  std::cout << test1.SerializeAsString() << "\n";
  return 0;
}
