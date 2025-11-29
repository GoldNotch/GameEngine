#pragma once
#include <GameFramework_def.h>

#include <array>
#include <string_view>

namespace GameFramework
{
using UUID = std::array<uint32_t, 9>;
static constexpr UUID UuidNull{0, 0, 0, 0, 0, 0, 0, 0, 0};

namespace Utils
{

GAME_FRAMEWORK_API UUID GenerateUUID();

inline std::string_view StringifyUUID(const UUID & uuid) noexcept
{
  return std::string_view{reinterpret_cast<const char *>(uuid.data()), 36};
}
} // namespace Utils
} // namespace GameFramework
