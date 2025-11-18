#pragma once
#include <Input/InputController.hpp>

namespace GlfwWindowsPlugin
{
GameFramework::PressState ConvertPressState(int action, int mods);
GameFramework::InputButton ConvertMouseButtonCode(int code);
GameFramework::InputButton ConvertKeyboardButtonCode(int code);
int ConvertJoystickButton2Code(GameFramework::InputButton btn);
int ConvertJoystickAxis2Code(GameFramework::InputAxis axis);
int InputDevice2JoystickId(GameFramework::InputDevice dev) noexcept;
GameFramework::InputDevice JoystickId2InputDevice(int jid) noexcept;
} // namespace GlfwWindowsPlugin
