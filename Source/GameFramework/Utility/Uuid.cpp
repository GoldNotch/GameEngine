#include "Uuid.hpp"

#include <cassert>
#include <random>
#include <sstream>

namespace GameFramework::Utils
{
GAME_FRAMEWORK_API UUID GenerateUUID()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  int i;
  ss << std::hex;
  for (i = 0; i < 8; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++)
  {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++)
  {
    ss << dis(gen);
  };
  UUID res;
  auto str = ss.str();
  assert(str.size() == 36);
  std::memcpy(res.data(), str.c_str(), 36);
  return res;
}

} // namespace GameFramework::Utils
