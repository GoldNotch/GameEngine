#pragma once
#include <GameFramework_def.h>

#include <array>
#include <optional>
#include <span>
#include <string>

namespace GameFramework
{

struct GAME_FRAMEWORK_API Uuid final
{
  Uuid();
  explicit Uuid(std::span<const char, 16> bytes16);
  explicit Uuid(std::span<const std::byte, 16> bytes16);
  ~Uuid();

  bool operator==(const Uuid & rhs) const noexcept;
  bool operator!=(const Uuid & rhs) const noexcept;
  bool operator<(const Uuid & rhs) const noexcept;

  size_t Hash() const noexcept;
  std::string ToString() const noexcept;

  static std::optional<Uuid> MakeFromString(const std::string_view & str);
  static Uuid MakeRandomUuid();

private:
  std::byte m_bytes[16];
};

} // namespace GameFramework


namespace std
{
template<>
struct hash<GameFramework::Uuid>
{
  size_t operator()(const GameFramework::Uuid & uuid) const noexcept { return uuid.Hash(); }
};
} // namespace std
