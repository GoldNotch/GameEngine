#include "GlfwInput.hpp"

#include <GLFW/glfw3.h>

namespace GlfwWindowsPlugin
{
GameFramework::PressState ConvertPressState(int action, int mods)
{
  GameFramework::PressState result = GameFramework::PressState::RELEASED;
  switch (action)
  {
    case GLFW_PRESS:
      result = GameFramework::PressState::JUST_PRESSED;
      break;
    case GLFW_RELEASE:
      result = GameFramework::PressState::RELEASED;
      break;
    case GLFW_REPEAT:
      result = GameFramework::PressState::PRESSING;
      break;
  }
  result = result | static_cast<GameFramework::PressState>(mods << 2);
  return result;
}

GameFramework::InputButton ConvertMouseButtonCode(int code)
{
  return static_cast<GameFramework::InputButton>(code);
}

GameFramework::InputButton ConvertKeyboardButtonCode(int code)
{
  return static_cast<GameFramework::InputButton>(code);
}

int ConvertJoystickButton2Code(GameFramework::InputButton btn)
{
  if (btn >= GameFramework::InputButton::GAMEPAD_FIRST_BUTTON &&
      btn <= GameFramework::InputButton::GAMEPAD_LAST_BUTTON)
    return static_cast<int>(btn) -
           static_cast<int>(GameFramework::InputButton::GAMEPAD_FIRST_BUTTON);

  if (btn >= GameFramework::InputButton::JOYSTICK_FIRST_BUTTON &&
      btn <= GameFramework::InputButton::JOYSTICK_LAST_BUTTON)
    return static_cast<int>(btn) -
           static_cast<int>(GameFramework::InputButton::JOYSTICK_FIRST_BUTTON);

  return GLFW_KEY_UNKNOWN;
}

int ConvertJoystickAxis2Code(GameFramework::InputAxis axis)
{
  if (axis >= GameFramework::InputAxis::GAMEPAD_FIRST_AXIS &&
      axis <= GameFramework::InputAxis::GAMEPAD_LAST_AXIS)
    return static_cast<int>(axis) - static_cast<int>(GameFramework::InputAxis::GAMEPAD_FIRST_AXIS);

  if (axis >= GameFramework::InputAxis::JOYSTICK_FIRST_AXIS &&
      axis <= GameFramework::InputAxis::JOYSTICK_LAST_AXIS)
    return static_cast<int>(axis) - static_cast<int>(GameFramework::InputAxis::JOYSTICK_FIRST_AXIS);

  return GLFW_KEY_UNKNOWN;
}

int InputDevice2JoystickId(GameFramework::InputDevice dev) noexcept
{
  using namespace GameFramework;
  if (dev == InputDevice::KEYBOARD || dev == InputDevice::MOUSE)
    return -1;
  else
    return (static_cast<int>(dev) >> 2) - 2;
}

GameFramework::InputDevice JoystickId2InputDevice(int jid) noexcept
{
  return static_cast<GameFramework::InputDevice>((jid + 2) << 2);
}

} // namespace GlfwWindowsPlugin
