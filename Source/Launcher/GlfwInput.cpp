#include "GlfwInput.hpp"

#include <GLFW/glfw3.h>

namespace Utils
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

GameFramework::InputButton ConvertGamepadButtonCode(int code)
{
  return static_cast<GameFramework::InputButton>(code + GLFW_MOUSE_BUTTON_LAST);
}

} // namespace Utils
