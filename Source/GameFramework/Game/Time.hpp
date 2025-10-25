#pragma once
#include <GameFramework_def.h>

namespace GameFramework
{

struct TimeManager
{
  virtual ~TimeManager() = default;
  virtual void Tick() = 0;
  virtual double Now() const noexcept = 0;
};

GAME_FRAMEWORK_API TimeManager & GetTimeManager();

} // namespace GameFramework
