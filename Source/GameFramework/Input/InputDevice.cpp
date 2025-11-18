#include "InputDevice.hpp"

namespace GameFramework
{

bool IsAxisValueValid(AxisValue val) noexcept
{
  return std::isinf(val);
}


InputDevice GetDeviceByButton(InputButton button) noexcept
{
  if (button >= InputButton::MOUSE_BUTTON_1 && button <= InputButton::MOUSE_LAST_BUTTON)
    return InputDevice::MOUSE;
  if (button >= InputButton::KEY_SPACE && button <= InputButton::KEYBOARD_LAST_BUTTON)
    return InputDevice::KEYBOARD;

  if (button >= InputButton::GAMEPAD_BUTTON_A && button <= InputButton::GAMEPAD_LAST_BUTTON)
    return InputDevice::ANY_JOYSTICK;
  if (button >= InputButton::JOYSTICK_BUTTON_1 && button <= InputButton::JOYSTICK_LAST_BUTTON)
    return InputDevice::ANY_JOYSTICK;

  return InputDevice::UNKNOWN;
}

InputDevice GetDeviceByAxis(InputAxis axis) noexcept
{
  switch (axis)
  {
    case InputAxis::MOUSE_CURSOR_X:
    case InputAxis::MOUSE_CURSOR_Y:
      return InputDevice::MOUSE;
    case InputAxis::GAMEPAD_LEFT_STICK_X:
    case InputAxis::GAMEPAD_LEFT_STICK_Y:
    case InputAxis::GAMEPAD_RIGHT_STICK_X:
    case InputAxis::GAMEPAD_RIGHT_STICK_Y:
    case InputAxis::GAMEPAD_LEFT_TRIGGER:
    case InputAxis::GAMEPAD_RIGHT_TRIGGER:
    case InputAxis::JOYSTICK_AXIS_1:
    case InputAxis::JOYSTICK_AXIS_2:
    case InputAxis::JOYSTICK_AXIS_3:
    case InputAxis::JOYSTICK_AXIS_4:
    case InputAxis::JOYSTICK_AXIS_5:
    case InputAxis::JOYSTICK_AXIS_6:
      return InputDevice::ANY_JOYSTICK;
    default:
      return InputDevice::UNKNOWN;
  }
}
} // namespace GameFramework
