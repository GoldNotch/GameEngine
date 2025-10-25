#pragma once
#include <Input/InputController.hpp>

namespace Utils
{
GameFramework::PressState ConvertPressState(int action, int mods);
GameFramework::InputButton ConvertMouseButtonCode(int code);
GameFramework::InputButton ConvertKeyboardButtonCode(int code);
GameFramework::InputButton ConvertGamepadButtonCode(int code);
} // namespace Utils
