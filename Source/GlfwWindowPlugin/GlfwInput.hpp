#pragma once
#include <Input/InputController.hpp>

namespace GlfwWindowsPlugin
{
GameFramework::PressState ConvertPressState(int action, int mods);
GameFramework::InputButton ConvertMouseButtonCode(int code);
GameFramework::InputButton ConvertKeyboardButtonCode(int code);
int ConvertGamepadButton2Code(GameFramework::InputButton btn);
int InputDevice2GamepadId(GameFramework::InputDevice dev) noexcept;
} // namespace GlfwWindowsPlugin
