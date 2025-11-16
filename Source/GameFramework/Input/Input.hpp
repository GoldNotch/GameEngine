#pragma once
#include <GameFramework_def.h>

#include <string>
#include <variant>

#include <Game/Math.hpp>

namespace GameFramework
{

enum class ActionType : uint8_t
{
  Event,     ///< fires once when keyset is just pressed
  Continous, ///< fires every time when keyset is pressed
  Axis       ///< fires all the time with some variadic value
};

enum class PlayerID : int8_t
{
  ANY = -1,
  PLAYER0,
  PLAYER1,
  PLAYER2,
  PLAYER3,
  PLAYER4,
  PLAYER5,
  PLAYER6,
  PLAYER7,
  PLAYER8,
  PLAYER9,
  PLAYER10,

  MAXPLAYER = 127
};

/// fires once when you press some key
/// might have difficult condition
struct EventAction final
{
  int code = 0; ///< code ID of the action, use enum to define it
  PlayerID playerId = PlayerID::ANY;
};

/// continous-in-time action, has a duration. Fires while you press a button.
/// This event might not have a hard condition like time-delay or double-click.
/// While you're pressing the right set of keys this action is firing.
struct ContinousAction final
{
  int code = 0; ///< code ID of the action, use enum to define it
  PlayerID playerId = PlayerID::ANY;
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
  PlayerID playerId = PlayerID::ANY;
  Vec3f axisValue;
  Vec3f deltaValue;
};

using GameInputEvent = std::variant<EventAction, ContinousAction, AxisAction>;

struct InputBinding
{
  std::string name; ///< name of the action, can be "Jump", "MoveForward", etc
  int code = 0;     ///< code ID of the action, use enum to define it
  PlayerID playerId = PlayerID::ANY;
  std::string bindings; ///< string to declare keys that represents this action
  ActionType type = ActionType::Event;
};

} // namespace GameFramework
