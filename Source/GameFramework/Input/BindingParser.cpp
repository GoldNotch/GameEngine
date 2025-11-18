#include "BindingParser.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <stdexcept>

#include <GameFramework.hpp>
#include <Utility/StringUtils.hpp>

namespace GameFramework::details
{

/// all binding are delimited with ;
static constexpr char s_delimiter = ';';
static constexpr char s_addButton = '+';
static constexpr char s_modButton = '&';
static constexpr char s_nextButton = '>';

namespace
{
InputAxis ParseAxisName(std::string_view str) noexcept
{
  using AxisMap = std::unordered_map<std::string_view, InputAxis>;
  static const AxisMap s_axisMap = {
    // Mouse buttons
    {"MouseCursorX", InputAxis::MOUSE_CURSOR_X},
    {"MouseCursorY", InputAxis::MOUSE_CURSOR_Y},
    {"GamepadLeftStickX", InputAxis::GAMEPAD_LEFT_STICK_X},
    {"GamepadLeftStickY", InputAxis::GAMEPAD_LEFT_STICK_Y},
    {"GamepadRightStickX", InputAxis::GAMEPAD_RIGHT_STICK_X},
    {"GamepadRightStickY", InputAxis::GAMEPAD_RIGHT_STICK_Y},
    {"GamepadL2", InputAxis::GAMEPAD_LEFT_TRIGGER},
    {"GamepadR2", InputAxis::GAMEPAD_RIGHT_TRIGGER},
  };

  auto it = s_axisMap.find(Utils::Trim(str));
  return it == s_axisMap.end() ? InputAxis::UNKNOWN : it->second;
}

std::pair<InputDevice, InputAxis> ParseSingleAxis(std::string_view str) noexcept
{
  if (str.empty())
  {
    GameFramework::Log(GameFramework::LogMessageType::Error, "Expected axis's name");
    return {InputDevice::UNKNOWN, InputAxis::UNKNOWN};
  }
  auto axis = ParseAxisName(str);
  auto foundDevice = GetDeviceByAxis(axis);
  return {foundDevice, axis};
}

std::pair<InputDevice, AxesSuperposition> ParseAxisBinding(std::string_view str) noexcept
{
  if (str.empty())
  {
    GameFramework::Log(GameFramework::LogMessageType::Error, "Expected axis's superposition");
    return {InputDevice::UNKNOWN, {InputAxis::UNKNOWN}};
  }
  InputDevice foundDevice = InputDevice::UNKNOWN;
  AxesSuperposition superposition{InputAxis::UNKNOWN, InputAxis::UNKNOWN, InputAxis::UNKNOWN};
  auto singleAxes = Utils::Split(str, s_addButton);
  size_t i = 0;
  for (auto && axisStr : singleAxes)
  {
    auto [parsedDevice, axis] = ParseSingleAxis(axisStr);
    if (parsedDevice != InputDevice::UNKNOWN && axis != InputAxis::UNKNOWN)
    {
      if (foundDevice == InputDevice::UNKNOWN)
        foundDevice = parsedDevice;
      assert(foundDevice == parsedDevice); // all axes should come from one device
      superposition[i++] = axis;
    }
  }
  return {foundDevice, superposition};
}
} // namespace

namespace
{

InputButton ParseButtonName(std::string_view str) noexcept
{
  using ButtonsMap = std::unordered_map<std::string_view, InputButton>;
  static const ButtonsMap s_buttonsMap = {
    // Mouse buttons
    {"MouseButton1", InputButton::MOUSE_BUTTON_1},
    {"MouseButton2", InputButton::MOUSE_BUTTON_2},
    {"MouseButton3", InputButton::MOUSE_BUTTON_3},
    {"MouseButton4", InputButton::MOUSE_BUTTON_4},
    {"MouseButton5", InputButton::MOUSE_BUTTON_5},
    {"MouseButton6", InputButton::MOUSE_BUTTON_6},
    {"MouseButton7", InputButton::MOUSE_BUTTON_7},
    {"MouseButton8", InputButton::MOUSE_BUTTON_8},
    {"MouseButtonLeft", InputButton::MOUSE_BUTTON_LEFT},
    {"MouseButtonRight", InputButton::MOUSE_BUTTON_RIGHT},
    {"MouseButtonMiddle", InputButton::MOUSE_BUTTON_MIDDLE},

    // Gamepad buttons
    {"GamepadA", InputButton::GAMEPAD_BUTTON_A},
    {"GamepadB", InputButton::GAMEPAD_BUTTON_B},
    {"GamepadX", InputButton::GAMEPAD_BUTTON_X},
    {"GamepadY", InputButton::GAMEPAD_BUTTON_Y},
    {"GamepadL1", InputButton::GAMEPAD_BUTTON_LEFT_BUMPER},
    {"GamepadR1", InputButton::GAMEPAD_BUTTON_RIGHT_BUMPER},
    {"GamepadBack", InputButton::GAMEPAD_BUTTON_BACK},
    {"GamepadStart", InputButton::GAMEPAD_BUTTON_START},
    {"GamepadGuide", InputButton::GAMEPAD_BUTTON_GUIDE},
    {"GamepadL3", InputButton::GAMEPAD_BUTTON_LEFT_THUMB},
    {"GamepadR3", InputButton::GAMEPAD_BUTTON_RIGHT_THUMB},
    {"GamepadDpadUp", InputButton::GAMEPAD_BUTTON_DPAD_UP},
    {"GamepadDpadRight", InputButton::GAMEPAD_BUTTON_DPAD_RIGHT},
    {"GamepadDpadDown", InputButton::GAMEPAD_BUTTON_DPAD_DOWN},
    {"GamepadDpadLeft", InputButton::GAMEPAD_BUTTON_DPAD_LEFT},

    // Keyboard letters
    {"KeyA", InputButton::KEY_A},
    {"KeyB", InputButton::KEY_B},
    {"KeyC", InputButton::KEY_C},
    {"KeyD", InputButton::KEY_D},
    {"KeyE", InputButton::KEY_E},
    {"KeyF", InputButton::KEY_F},
    {"KeyG", InputButton::KEY_G},
    {"KeyH", InputButton::KEY_H},
    {"KeyI", InputButton::KEY_I},
    {"KeyJ", InputButton::KEY_J},
    {"KeyK", InputButton::KEY_K},
    {"KeyL", InputButton::KEY_L},
    {"KeyM", InputButton::KEY_M},
    {"KeyN", InputButton::KEY_N},
    {"KeyO", InputButton::KEY_O},
    {"KeyP", InputButton::KEY_P},
    {"KeyQ", InputButton::KEY_Q},
    {"KeyR", InputButton::KEY_R},
    {"KeyS", InputButton::KEY_S},
    {"KeyT", InputButton::KEY_T},
    {"KeyU", InputButton::KEY_U},
    {"KeyV", InputButton::KEY_V},
    {"KeyW", InputButton::KEY_W},
    {"KeyX", InputButton::KEY_X},
    {"KeyY", InputButton::KEY_Y},
    {"KeyZ", InputButton::KEY_Z},

    // Keyboard digits
    {"Key0", InputButton::KEY_0},
    {"Key1", InputButton::KEY_1},
    {"Key2", InputButton::KEY_2},
    {"Key3", InputButton::KEY_3},
    {"Key4", InputButton::KEY_4},
    {"Key5", InputButton::KEY_5},
    {"Key6", InputButton::KEY_6},
    {"Key7", InputButton::KEY_7},
    {"Key8", InputButton::KEY_8},
    {"Key9", InputButton::KEY_9},

    // Function keys
    {"KeyF1", InputButton::KEY_F1},
    {"KeyF2", InputButton::KEY_F2},
    {"KeyF3", InputButton::KEY_F3},
    {"KeyF4", InputButton::KEY_F4},
    {"KeyF5", InputButton::KEY_F5},
    {"KeyF6", InputButton::KEY_F6},
    {"KeyF7", InputButton::KEY_F7},
    {"KeyF8", InputButton::KEY_F8},
    {"KeyF9", InputButton::KEY_F9},
    {"KeyF10", InputButton::KEY_F10},
    {"KeyF11", InputButton::KEY_F11},
    {"KeyF12", InputButton::KEY_F12},

    // Special keys
    {"KeySpace", InputButton::KEY_SPACE},
    {"KeyEnter", InputButton::KEY_ENTER},
    {"KeyEscape", InputButton::KEY_ESCAPE},
    {"KeyTab", InputButton::KEY_TAB},
    {"KeyBackspace", InputButton::KEY_BACKSPACE},
    {"KeyInsert", InputButton::KEY_INSERT},
    {"KeyDelete", InputButton::KEY_DELETE},
    {"KeyHome", InputButton::KEY_HOME},
    {"KeyEnd", InputButton::KEY_END},
    {"KeyPageUp", InputButton::KEY_PAGE_UP},
    {"KeyPageDown", InputButton::KEY_PAGE_DOWN},

    // Arrow keys
    {"KeyLeft", InputButton::KEY_LEFT},
    {"KeyRight", InputButton::KEY_RIGHT},
    {"KeyUp", InputButton::KEY_UP},
    {"KeyDown", InputButton::KEY_DOWN},

    // Modifier keys
    {"KeyLeftShift", InputButton::KEY_LEFT_SHIFT},
    {"KeyRightShift", InputButton::KEY_RIGHT_SHIFT},
    {"KeyLeftCtrl", InputButton::KEY_LEFT_CONTROL},
    {"KeyRightCtrl", InputButton::KEY_RIGHT_CONTROL},
    {"KeyLeftAlt", InputButton::KEY_LEFT_ALT},
    {"KeyRightAlt", InputButton::KEY_RIGHT_ALT},
    {"KeyLeftSuper", InputButton::KEY_LEFT_SUPER},
    {"KeyRightSuper", InputButton::KEY_RIGHT_SUPER},

    // Keypad
    {"KeyNumpad0", InputButton::KEY_KP_0},
    {"KeyNumpad1", InputButton::KEY_KP_1},
    {"KeyNumpad2", InputButton::KEY_KP_2},
    {"KeyNumpad3", InputButton::KEY_KP_3},
    {"KeyNumpad4", InputButton::KEY_KP_4},
    {"KeyNumpad5", InputButton::KEY_KP_5},
    {"KeyNumpad6", InputButton::KEY_KP_6},
    {"KeyNumpad7", InputButton::KEY_KP_7},
    {"KeyNumpad8", InputButton::KEY_KP_8},
    {"KeyNumpad9", InputButton::KEY_KP_9},
    {"KeyNumpadAdd", InputButton::KEY_KP_ADD},
    {"KeyNumpadSubtract", InputButton::KEY_KP_SUBTRACT},
    {"KeyNumpadMultiply", InputButton::KEY_KP_MULTIPLY},
    {"KeyNumpadDivide", InputButton::KEY_KP_DIVIDE},
    {"KeyNumpadDecimal", InputButton::KEY_KP_DECIMAL},
    {"KeyNumpadEnter", InputButton::KEY_KP_ENTER},
    {"KeyNumpadEqual", InputButton::KEY_KP_EQUAL},
  };

  auto it = s_buttonsMap.find(Utils::Trim(str));
  return it == s_buttonsMap.end() ? InputButton::UNKNOWN : it->second;
}

PressState ParseButtonModifier(std::string_view mod) noexcept
{
  static const std::unordered_map<std::string_view, PressState>
    s_modifiersMap{{"Shift", PressState::SHIFT},        {"Alt", PressState::ALT},
                   {"Ctrl", PressState::CTRL},          {"Super", PressState::SUPER},
                   {"CapsLock", PressState::CAPS_LOCK}, {"NumLock", PressState::NUM_LOCK}};

  auto it = s_modifiersMap.find(Utils::Trim(mod));
  return it == s_modifiersMap.end() ? PressState::RELEASED : it->second;
}

std::tuple<InputDevice, InputButton, PressState> ParseButtonPressState(
  std::string_view str) noexcept
{
  if (str.empty())
  {
    GameFramework::Log(GameFramework::LogMessageType::Error, "Expected button or modifier");
    return {InputDevice::UNKNOWN, InputButton::UNKNOWN, PressState::RELEASED};
  }

  InputDevice resultDev = InputDevice::UNKNOWN;
  InputButton resultBtn = InputButton::UNKNOWN;
  PressState modsRequired = PressState::JUST_PRESSED;
  auto mods = Utils::Split(str, s_modButton);
  for (auto && btnOrMod : mods)
  {
    if (resultBtn == InputButton::UNKNOWN)
    {
      InputButton btn = ParseButtonName(btnOrMod);
      InputDevice foundDevice = GetDeviceByButton(btn);
      if (btn != InputButton::UNKNOWN)
      {
        resultBtn = btn;
        resultDev = foundDevice;
        continue;
      }
    }

    PressState state = ParseButtonModifier(btnOrMod);
    if (state != PressState::RELEASED)
    {
      modsRequired = modsRequired | state;
      continue;
    }

    Log(LogMessageType::Warning, "Unknown button or mod  - ", str);
  }

  return {resultDev, resultBtn, modsRequired};
}

std::pair<InputDevice, ButtonsChord> TryParseButtonsChord(std::string_view str)
{
  if (str.empty())
  {
    GameFramework::Log(GameFramework::LogMessageType::Error, "Expected button's chord");
    return {InputDevice::UNKNOWN, {}};
  }
  auto separatedButtons = Utils::Split(str, s_addButton);
  InputDevice chordDevice = InputDevice::UNKNOWN;
  details::ButtonsChord chord;
  chord.reserve(separatedButtons.size());
  for (auto && btnAndModStr : separatedButtons)
  {
    auto [device, btn, state] = ParseButtonPressState(btnAndModStr);
    if (btn != InputButton::UNKNOWN)
    {
      if (chordDevice == InputDevice::UNKNOWN)
        chordDevice = device; // first parsed device is a device for a whole chord
      if (chordDevice != device)
        Log(LogMessageType::Error, "Expected that all buttons in chord will be from one device");
      else
        chord.push_back({btn, state});
    }
  }
  return {chordDevice, chord};
}

} // namespace

BindingParser::BindingParser(std::string_view str)
{
  if (str.empty())
    throw std::runtime_error("Binding expression was empty");


  auto singleConditions = Utils::Split(str, s_delimiter);
  m_parsedBinding.clear();

  for (auto && conditionStr : singleConditions)
  {
    auto [parsedAxisDevice, parsedAxisSuper] = ParseAxisBinding(conditionStr);
    if (parsedAxisDevice != InputDevice::UNKNOWN)
    {
      m_parsedBinding[parsedAxisDevice].first.push_back(parsedAxisSuper);
      continue;
    }

    auto [parsedBtnDevice, parsedChord] = TryParseButtonsChord(conditionStr);
    if (parsedBtnDevice != InputDevice::UNKNOWN && !parsedChord.empty())
    {
      m_parsedBinding[parsedBtnDevice].second.emplace_back(std::move(parsedChord));
      continue;
    }

    GameFramework::Log(GameFramework::LogMessageType::Error, "Expected axis or button's chord");
  }
}

AxesCondition BindingParser::GetAxesResult(InputDevice device) const
{
  auto it = m_parsedBinding.find(device);
  for (auto [dev, conditions] : m_parsedBinding)
  {
    if (!!(dev & device))
      return conditions.first;
  }
  return {};
}

ButtonsCondition BindingParser::GetButtonsResult(InputDevice device) const
{
  auto it = m_parsedBinding.find(device);
  for (auto [dev, conditions] : m_parsedBinding)
  {
    if (!!(dev & device))
      return conditions.second;
  }
  return {};
}

} // namespace GameFramework::details
