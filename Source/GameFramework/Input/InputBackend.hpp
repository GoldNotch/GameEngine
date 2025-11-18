#pragma once
#include <Input/InputDevice.hpp>

namespace GameFramework
{

struct InputController;

/// @brief provides low-level input events
struct InputBackend
{
  virtual ~InputBackend() = default;

  /// @brief bind high-level controller to backend
  /// @param controller
  virtual void BindController(InputController * controller) = 0;

  /// @brief check state of button on current device
  /// @param device - id of device
  /// @param btn - id of button
  /// @return state of button (pressed, released ,etc)
  /// IF device is not connected or hasn't so button, return RELEASED
  virtual PressState CheckButtonState(InputDevice device, InputButton btn) const noexcept = 0;

  /// @brief check state of axis on current device
  /// @param device - id of device
  /// @param axis - id of axis
  /// @return value of axis (if device is not connected or it hasn't so axis, return NAN)
  virtual AxisValue CheckAxisState(InputDevice device, InputAxis axis) const noexcept = 0;

  /// @brief get description of input device
  virtual InputDeviceDescription GetInputDeviceDescription(InputDevice device) const noexcept = 0;
};
} // namespace GameFramework
