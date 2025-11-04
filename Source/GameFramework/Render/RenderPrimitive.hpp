#pragma once
#include <GameFramework_def.h>

#include <memory>
#include <span>
#include <type_traits>

namespace GameFramework
{
struct GAME_FRAMEWORK_API IRenderPrimitive
{
  virtual ~IRenderPrimitive() = default;
  virtual size_t Hash() const noexcept = 0;
};

} // namespace GameFramework

namespace std
{
template<typename PrimT>
  requires(std::is_base_of_v<GameFramework::IRenderPrimitive, PrimT>)
struct hash<PrimT>
{
  std::size_t operator()(const PrimT & prim) const noexcept { return prim.Hash(); }
};
} // namespace std
