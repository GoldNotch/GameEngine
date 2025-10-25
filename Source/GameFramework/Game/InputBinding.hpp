#pragma once
#include <GameFramework_def.h>

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

#include <Game/Input.hpp>
#include <Game/InputDevice.hpp>

namespace GameFramework
{

struct InputBinding
{
  std::string name;     ///< name of the action, can be "Jump", "MoveForward", etc
  int code = 0;         ///< code ID of the action, use enum to define it
  std::string bindings; ///< string to declare keys that represents this action
  ActionType type = ActionType::Event;
};

namespace details
{
using ButtonSimpleCondition = std::pair<InputButton, PressState>;
using BindingConjunction = std::vector<ButtonSimpleCondition>;
using BindingDisjunction = std::vector<BindingConjunction>;
} // namespace details

class ActionGenerator
{
public:
  using CheckButtonStateFunc = std::function<PressState(InputButton)>;
  using GetTimeFunc = std::function<double()>;

  explicit ActionGenerator(const InputBinding & binding);
  /// set delegate to check press state of buttonk
  void SetCheckButtonStateFunc(CheckButtonStateFunc && checkState) noexcept;

  /// update state of action
  void TickAction(double currentTime);
  /// get generated action if it's active
  std::optional<GameInputEvent> GetAction() const noexcept;

private:
  InputBinding m_binding;
  bool m_isActive = false;
  double m_activeTimestamp = 0.0;
  double m_activeDuration = 0.0;
  details::BindingDisjunction m_builtCondition;

  CheckButtonStateFunc m_checkState;

private:
  bool IsActionActive() const;
};

} // namespace GameFramework
