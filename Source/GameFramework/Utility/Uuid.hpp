#pragma once
#include <GameFramework_def.h>

#include <span>
#include <string>

namespace GameFramework
{

GAME_FRAMEWORK_API struct Uuid final
{
  Uuid();
  explicit Uuid(const std::string & str);
  explicit Uuid(const std::span<char, 16> & bytes16);
  explicit Uuid(const std::span<uint8_t, 16> & bytes16);
  ~Uuid();

  bool operator==(const Uuid & rhs) const noexcept;
  bool operator!=(const Uuid & rhs) const noexcept;
  bool operator<(const Uuid & rhs) const noexcept;

  std::string ToString() const noexcept;

  static Uuid MakeRandomUuid();

private:
  uint32_t x[4];
};

} // namespace GameFramework
