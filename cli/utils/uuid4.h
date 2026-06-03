#ifndef CLI_UTILS_UUID4_H_
#define CLI_UTILS_UUID4_H_

#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace vibra {

namespace uuid4 {
std::string Generate() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  ss << std::hex;
  for (int i = 0; i < 8; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (int i = 0; i < 4; i++) {
    ss << dis(gen);
  }
  ss << "-4";
  for (int i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for (int i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (int i = 0; i < 12; i++) {
    ss << dis(gen);
  }

  return ss.str();  // RVO. Guaranteed copy elision since C++17
}
}  // namespace uuid4

}  // namespace vibra

#endif  // CLI_UTILS_UUID4_H_
