#pragma once
#include <functional>
#include <memory>
#include <optional>

#include <Input/Input.hpp>
#include <Input/InputDevice.hpp>

namespace GameFramework::details
{
/// @brief a set of axes unites into one action event
///         There is only 3 axes can be superposited
using AxesSuperposition = std::array<InputAxis, AxesSuperpositionLimit>;

/// @brief condition for axis action. can be activated with multiple axes
///        For example rotate camera you can with mouse cursor or gamepad's stick
using AxesCondition = std::vector<AxesSuperposition>;

/// @brief condition for pressing a button.
/// Contains the button and state the button should be in to activate the action
using ButtonPressState = std::pair<InputButton, PressState>;

/// @brief a set of buttons which are should be pressed at one time to activate the action
///     For example: W+X - sprint
using ButtonsChord = std::vector<ButtonPressState>;

/// @brief condition for button action. Can be activated with multiple button's sets
///         for example: Go forward is W or Up or
using ButtonsCondition = std::vector<ButtonsChord>;

/// @brief watches on one ActionEvent
struct InputProcessor
{
  virtual ~InputProcessor() = default;
  /// @brief updates state of processor and generate actions if need it
  /// @param currentTime
  virtual void TickAction(double currentTime) = 0;
  /// @brief get generated action if it's active
  virtual std::optional<GameInputEvent> GetAction() const noexcept = 0;
  /// @brief get device to be listened
  /// @return id of device
  virtual InputDevice GetDevice() const noexcept = 0;
};

using InputProcessorUPtr = std::unique_ptr<InputProcessor>;

InputProcessorUPtr CreateButtonProcessor(ActionType actionType, int actionCode, InputDevice device,
                                         ButtonsCondition condition,
                                         CheckButtonStateFunc checkButtonState);
InputProcessorUPtr CreateAxisProcessor(ActionType actionType, int actionCode, InputDevice device,
                                       AxesCondition condition, CheckAxisStateFunc checkAxisState);
} // namespace GameFramework::details
