#include <iostream>
#include <string>

#include "hello.pb.h"

int main() {
  hello::TestHello test;
  test.set_world("world");
  std::cout << test.SerializePartialAsString();
  return 0;
}
