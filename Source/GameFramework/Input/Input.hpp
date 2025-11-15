#pragma once
#include <GameFramework_def.h>

#include <string>
#include <variant>

//TODO: rename to InputAction.hpp
namespace GameFramework
{

enum class ActionType : uint8_t
{
  Event,     ///< fires once when keyset is just pressed
  Continous, ///< fires every time when keyset is pressed
  Axis       ///< fires all the time with some variadic value
};

/// fires once when you press some key
/// might have difficult condition
struct EventAction
{
  int code = 0; ///< code ID of the action, use enum to define it
};

/// continous-in-time action, has a duration. Fires while you press a button.
/// This event might not have a hard condition like time-delay or double-click.
/// While you're pressing the right set of keys this action is firing.
struct ContinousAction
{
  int code = 0; ///< code ID of the action, use enum to define it
  double activeStart;
  double activeDuration;
};

/// permanent action, fires every time
/// but it has a vector of changing some value (derivative)
/// support 3d axis
/// Also it might have a threshold value when this event is never happend
struct AxisAction
{
  int code = 0; ///< code ID of the action, use enum to define it
  float xAxisValue = 0.0f;
  float dxAxisValue = 0.0f;
  float yAxisValue = 0.0f;
  float dyAxisValue = 0.0f;
  float zAxisValue = 0.0f;
  float dzAxisValue = 0.0f;
};

using GameInputEvent = std::variant<EventAction, ContinousAction, AxisAction>;


} // namespace GameFramework
