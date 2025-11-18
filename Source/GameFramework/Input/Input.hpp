#pragma once
#include <GameFramework_def.h>

#include <string>
#include <variant>

#include <Input/InputDevice.hpp>

namespace GameFramework
{

enum class ActionType : uint8_t
{
  Event,     ///< fires once when keyset is just pressed
  Continous, ///< fires every time when keyset is pressed
  Axis       ///< fires all the time with some variadic value
};

/// @brief fires once when you press some key
///         might have difficult condition
struct EventAction final
{
  int code = 0; ///< code ID of the action, use enum to define it
  InputDevice device = InputDevice::UNKNOWN;
};

/// continous-in-time action, has a duration. Fires while you press a button.
/// This event might not have a hard condition like time-delay or double-click.
/// While you're pressing the right set of keys this action is firing.
struct ContinousAction final
{
  int code = 0; ///< code ID of the action, use enum to define it
  InputDevice device = InputDevice::UNKNOWN;
  double activeStart;
  double activeDuration;
};

/// permanent action, fires every time
/// but it has a vector of changing some value (derivative)
/// support 3d axis
/// Also it might have a threshold value when this event is never happend
struct AxisAction final
{
  int code = 0; ///< code ID of the action, use enum to define it
  InputDevice device = InputDevice::UNKNOWN;
  AxesValue axisValue;
  AxesValue deltaValue;
};

using GameInputEvent = std::variant<EventAction, ContinousAction, AxisAction>;

struct InputBinding
{
  std::string name;     ///< name of the action, can be "Jump", "MoveForward", etc
  int code = 0;         ///< code ID of the action, use enum to define it
  std::string bindings; ///< string to declare keys that represents this action
  ActionType type = ActionType::Event;
};

} // namespace GameFramework
