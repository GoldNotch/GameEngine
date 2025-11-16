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

int ConvertGamepadButton2Code(GameFramework::InputButton btn)
{
  if (btn < GameFramework::InputButton::GAMEPAD_BUTTON_A ||
      btn > GameFramework::InputButton::GAMEPAD_BUTTON_DPAD_LEFT)
    return GLFW_KEY_UNKNOWN;

  return static_cast<int>(btn) - static_cast<int>(GameFramework::InputButton::GAMEPAD_BUTTON_A);
}

int InputDevice2GamepadId(GameFramework::InputDevice dev) noexcept
{
  using namespace GameFramework;
  if (dev == InputDevice::KEYBOARD_MOUSE)
    return -1;
  else
    return static_cast<int>(dev) >> 1;
}

} // namespace GlfwWindowsPlugin
