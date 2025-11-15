#include "InputBinding.hpp"

#include <algorithm>
#include <chrono>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include <GameFramework.hpp>
#include <Utility/StringUtils.hpp>

namespace GameFramework::details
{
/// all binding are delimited with ;
static constexpr char s_delimiter = ';';
static constexpr char s_addButton = '+';
static constexpr char s_modButton = '&';
static constexpr char s_nextButton = '>';

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

std::string StringifyInputButton(InputButton button)
{
  using ButtonsMap = std::unordered_map<InputButton, std::string>;
  static const ButtonsMap s_buttonsMap = {
    // Mouse buttons
    {InputButton::MOUSE_BUTTON_1, "MouseButton1"},
    {InputButton::MOUSE_BUTTON_2, "MouseButton2"},
    {InputButton::MOUSE_BUTTON_3, "MouseButton3"},
    {InputButton::MOUSE_BUTTON_4, "MouseButton4"},
    {InputButton::MOUSE_BUTTON_5, "MouseButton5"},
    {InputButton::MOUSE_BUTTON_6, "MouseButton6"},
    {InputButton::MOUSE_BUTTON_7, "MouseButton7"},
    {InputButton::MOUSE_BUTTON_8, "MouseButton8"},
    {InputButton::MOUSE_BUTTON_LEFT, "MouseButtonLeft"},
    {InputButton::MOUSE_BUTTON_RIGHT, "MouseButtonRight"},
    {InputButton::MOUSE_BUTTON_MIDDLE, "MouseButtonMiddle"},

    // Gamepad buttons
    {InputButton::GAMEPAD_BUTTON_A, "GamepadA"},
    {InputButton::GAMEPAD_BUTTON_B, "GamepadB"},
    {InputButton::GAMEPAD_BUTTON_X, "GamepadX"},
    {InputButton::GAMEPAD_BUTTON_Y, "GamepadY"},
    {InputButton::GAMEPAD_BUTTON_LEFT_BUMPER, "GamepadL1"},
    {InputButton::GAMEPAD_BUTTON_RIGHT_BUMPER, "GamepadR1"},
    {InputButton::GAMEPAD_BUTTON_BACK, "GamepadBack"},
    {InputButton::GAMEPAD_BUTTON_START, "GamepadStart"},
    {InputButton::GAMEPAD_BUTTON_GUIDE, "GamepadGuide"},
    {InputButton::GAMEPAD_BUTTON_LEFT_THUMB, "GamepadL3"},
    {InputButton::GAMEPAD_BUTTON_RIGHT_THUMB, "GamepadR3"},
    {InputButton::GAMEPAD_BUTTON_DPAD_UP, "GamepadDpadUp"},
    {InputButton::GAMEPAD_BUTTON_DPAD_RIGHT, "GamepadDpadRight"},
    {InputButton::GAMEPAD_BUTTON_DPAD_DOWN, "GamepadDpadDown"},
    {InputButton::GAMEPAD_BUTTON_DPAD_LEFT, "GamepadDpadLeft"},

    // Keyboard letters
    {InputButton::KEY_A, "KeyA"},
    {InputButton::KEY_B, "KeyB"},
    {InputButton::KEY_C, "KeyC"},
    {InputButton::KEY_D, "KeyD"},
    {InputButton::KEY_E, "KeyE"},
    {InputButton::KEY_F, "KeyF"},
    {InputButton::KEY_G, "KeyG"},
    {InputButton::KEY_H, "KeyH"},
    {InputButton::KEY_I, "KeyI"},
    {InputButton::KEY_J, "KeyJ"},
    {InputButton::KEY_K, "KeyK"},
    {InputButton::KEY_L, "KeyL"},
    {InputButton::KEY_M, "KeyM"},
    {InputButton::KEY_N, "KeyN"},
    {InputButton::KEY_O, "KeyO"},
    {InputButton::KEY_P, "KeyP"},
    {InputButton::KEY_Q, "KeyQ"},
    {InputButton::KEY_R, "KeyR"},
    {InputButton::KEY_S, "KeyS"},
    {InputButton::KEY_T, "KeyT"},
    {InputButton::KEY_U, "KeyU"},
    {InputButton::KEY_V, "KeyV"},
    {InputButton::KEY_W, "KeyW"},
    {InputButton::KEY_X, "KeyX"},
    {InputButton::KEY_Y, "KeyY"},
    {InputButton::KEY_Z, "KeyZ"},

    // Keyboard digits
    {InputButton::KEY_0, "Key0"},
    {InputButton::KEY_1, "Key1"},
    {InputButton::KEY_2, "Key2"},
    {InputButton::KEY_3, "Key3"},
    {InputButton::KEY_4, "Key4"},
    {InputButton::KEY_5, "Key5"},
    {InputButton::KEY_6, "Key6"},
    {InputButton::KEY_7, "Key7"},
    {InputButton::KEY_8, "Key8"},
    {InputButton::KEY_9, "Key9"},

    // Function keys
    {InputButton::KEY_F1, "KeyF1"},
    {InputButton::KEY_F2, "KeyF2"},
    {InputButton::KEY_F3, "KeyF3"},
    {InputButton::KEY_F4, "KeyF4"},
    {InputButton::KEY_F5, "KeyF5"},
    {InputButton::KEY_F6, "KeyF6"},
    {InputButton::KEY_F7, "KeyF7"},
    {InputButton::KEY_F8, "KeyF8"},
    {InputButton::KEY_F9, "KeyF9"},
    {InputButton::KEY_F10, "KeyF10"},
    {InputButton::KEY_F11, "KeyF11"},
    {InputButton::KEY_F12, "KeyF12"},

    // Special keys
    {InputButton::KEY_SPACE, "KeySpace"},
    {InputButton::KEY_ENTER, "KeyEnter"},
    {InputButton::KEY_ESCAPE, "KeyEscape"},
    {InputButton::KEY_TAB, "KeyTab"},
    {InputButton::KEY_BACKSPACE, "KeyBackspace"},
    {InputButton::KEY_INSERT, "KeyInsert"},
    {InputButton::KEY_DELETE, "KeyDelete"},
    {InputButton::KEY_HOME, "KeyHome"},
    {InputButton::KEY_END, "KeyEnd"},
    {InputButton::KEY_PAGE_UP, "KeyPageUp"},
    {InputButton::KEY_PAGE_DOWN, "KeyPageDown"},

    // Arrow keys
    {InputButton::KEY_LEFT, "KeyLeft"},
    {InputButton::KEY_RIGHT, "KeyRight"},
    {InputButton::KEY_UP, "KeyUp"},
    {InputButton::KEY_DOWN, "KeyDown"},

    // Modifier keys
    {InputButton::KEY_LEFT_SHIFT, "KeyLeftShift"},
    {InputButton::KEY_RIGHT_SHIFT, "KeyRightShift"},
    {InputButton::KEY_LEFT_CONTROL, "KeyLeftCtrl"},
    {InputButton::KEY_RIGHT_CONTROL, "KeyRightCtrl"},
    {InputButton::KEY_LEFT_ALT, "KeyLeftAlt"},
    {InputButton::KEY_RIGHT_ALT, "KeyRightAlt"},
    {InputButton::KEY_LEFT_SUPER, "KeyLeftSuper"},
    {InputButton::KEY_RIGHT_SUPER, "KeyRightSuper"},

    // Keypad
    {InputButton::KEY_KP_0, "KeyNumpad0"},
    {InputButton::KEY_KP_1, "KeyNumpad1"},
    {InputButton::KEY_KP_2, "KeyNumpad2"},
    {InputButton::KEY_KP_3, "KeyNumpad3"},
    {InputButton::KEY_KP_4, "KeyNumpad4"},
    {InputButton::KEY_KP_5, "KeyNumpad5"},
    {InputButton::KEY_KP_6, "KeyNumpad6"},
    {InputButton::KEY_KP_7, "KeyNumpad7"},
    {InputButton::KEY_KP_8, "KeyNumpad8"},
    {InputButton::KEY_KP_9, "KeyNumpad9"},
    {InputButton::KEY_KP_ADD, "KeyNumpadAdd"},
    {InputButton::KEY_KP_SUBTRACT, "KeyNumpadSubtract"},
    {InputButton::KEY_KP_MULTIPLY, "KeyNumpadMultiply"},
    {InputButton::KEY_KP_DIVIDE, "KeyNumpadDivide"},
    {InputButton::KEY_KP_DECIMAL, "KeyNumpadDecimal"},
    {InputButton::KEY_KP_ENTER, "KeyNumpadEnter"},
    {InputButton::KEY_KP_EQUAL, "KeyNumpadEqual"},
  };

  auto it = s_buttonsMap.find(button);
  return it != s_buttonsMap.end() ? it->second : "UNKNOWN_BUTTON";
}

details::ButtonSimpleCondition ParseButtonAndMods(std::string_view str)
{
  if (str.empty())
    throw std::runtime_error("Button or modifier expected");
  InputButton resultBtn = InputButton::UNKNOWN;
  PressState modsRequired = PressState::JUST_PRESSED;
  auto mods = Utils::Split(str, s_modButton);
  for (auto && mod : mods)
  {
    InputButton btn = ParseButtonName(mod);
    if (btn != InputButton::UNKNOWN)
    {
      if (resultBtn != InputButton::UNKNOWN)
        throw std::runtime_error("Expression must have only one button");
      resultBtn = btn;
      continue;
    }

    PressState state = ParseButtonModifier(mod);
    if (state != PressState::RELEASED)
    {
      modsRequired = modsRequired | state;
      continue;
    }
  }

  if (resultBtn == InputButton::UNKNOWN)
    throw std::runtime_error("Expression must contain one button declared");
  return {resultBtn, modsRequired};
}

details::BindingConjunction ParseBindingConjunction(std::string_view str)
{
  if (str.empty())
    throw std::runtime_error("Binding conjunction expected");
  details::BindingConjunction conjunction;
  auto buttons = Utils::Split(str, s_addButton);
  conjunction.reserve(buttons.size());
  for (auto && btn : buttons)
  {
    conjunction.push_back(ParseButtonAndMods(btn));
  }
  return conjunction;
}

details::BindingDisjunction ParseBindingString(std::string_view str)
{
  if (str.empty())
    throw std::runtime_error("Binding expression was empty");
  details::BindingDisjunction disjunction;
  auto conjunctions = Utils::Split(str, s_delimiter);
  disjunction.reserve(conjunctions.size());
  for (auto && conjun : conjunctions)
    disjunction.push_back(ParseBindingConjunction(conjun));

  return disjunction;
}

ActionGenerator::ActionGenerator(const InputBinding & binding)
  : m_binding(binding)
{
  details::BindingDisjunction builtResult;
  try
  {
    builtResult = ParseBindingString(binding.bindings);
  }
  catch (const std::exception & e)
  {
    Log(LogMessageType::Error, "Failed to built input binding string - ", e.what());
    return;
  }
  m_builtCondition = std::move(builtResult);
}

void ActionGenerator::SetCheckButtonStateFunc(CheckButtonStateFunc && checkState) noexcept
{
  m_checkState = std::move(checkState);
}

void ActionGenerator::TickAction(double currentTime)
{
  bool isActive = IsActionActive();
  if (m_isActive != isActive)
  {
    // if just activated when set time to currentTime
    m_activeTimestamp = !m_isActive && isActive ? currentTime : 0.0;
    m_activeDuration = 0.0;
  }
  else
  {
    m_activeDuration = currentTime - m_activeTimestamp;
  }
  m_isActive = isActive;
}

std::optional<GameInputEvent> ActionGenerator::GetAction() const noexcept
{
  if (m_isActive)
  {
    switch (m_binding.type)
    {
      case ActionType::Event:
        return m_activeDuration > 0.0 ? std::nullopt
                                      : std::make_optional(EventAction{m_binding.code});
      case ActionType::Continous:
        return ContinousAction{m_binding.code, m_activeTimestamp, m_activeDuration};
      case ActionType::Axis:
        return AxisAction{m_binding.code};
      default:
        Log(LogMessageType::Warning, "Unknown action type is fired - ", m_binding.name, L"(",
            static_cast<int>(m_binding.type), ")");
        return std::nullopt;
    }
  }
  return std::nullopt;
}

bool ActionGenerator::IsActionActive() const
{
  if (m_builtCondition.empty())
    return false;

  auto checkSimpleCondition = [this](const details::ButtonSimpleCondition & btn)
  {
    return m_checkState(btn.first) == btn.second;
  };
  auto checkButtonSet =
    [&checkSimpleCondition](const details::BindingConjunction & buttonSet) -> bool
  {
    return std::ranges::all_of(buttonSet, checkSimpleCondition);
  };

  return std::ranges::any_of(m_builtCondition, checkButtonSet);
}

} // namespace GameFramework::details
