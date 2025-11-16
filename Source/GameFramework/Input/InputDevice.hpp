#pragma once

#include <functional>
#include <optional>

#include <Utility/Utility.hpp>

namespace GameFramework
{

enum class InputButton : int
{
  UNKNOWN = -1,
  // mouse buttons
  MOUSE_BUTTON_1 = 0,
  MOUSE_BUTTON_2 = 1,
  MOUSE_BUTTON_3 = 2,
  MOUSE_BUTTON_4 = 3,
  MOUSE_BUTTON_5 = 4,
  MOUSE_BUTTON_6 = 5,
  MOUSE_BUTTON_7 = 6,
  MOUSE_BUTTON_8 = 7,
  MOUSE_BUTTON_LEFT = MOUSE_BUTTON_1,
  MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2,
  MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3,

  // gamepad buttons
  GAMEPAD_BUTTON_A = 8,
  GAMEPAD_BUTTON_B = 9,
  GAMEPAD_BUTTON_X = 10,
  GAMEPAD_BUTTON_Y = 11,
  GAMEPAD_BUTTON_LEFT_BUMPER = 12,
  GAMEPAD_BUTTON_RIGHT_BUMPER = 13,
  GAMEPAD_BUTTON_BACK = 14,
  GAMEPAD_BUTTON_START = 15,
  GAMEPAD_BUTTON_GUIDE = 16,
  GAMEPAD_BUTTON_LEFT_THUMB = 17,
  GAMEPAD_BUTTON_RIGHT_THUMB = 18,
  GAMEPAD_BUTTON_DPAD_UP = 19,
  GAMEPAD_BUTTON_DPAD_RIGHT = 20,
  GAMEPAD_BUTTON_DPAD_DOWN = 21,
  GAMEPAD_BUTTON_DPAD_LEFT = 22,

  // keyboard buttons
  KEY_SPACE = 32,
  KEY_APOSTROPHE = 39, /* ' */
  KEY_COMMA = 44,      /* , */
  KEY_MINUS = 45,      /* - */
  KEY_PERIOD = 46,     /* . */
  KEY_SLASH = 47,      /* / */
  KEY_0 = 48,
  KEY_1 = 49,
  KEY_2 = 50,
  KEY_3 = 51,
  KEY_4 = 52,
  KEY_5 = 53,
  KEY_6 = 54,
  KEY_7 = 55,
  KEY_8 = 56,
  KEY_9 = 57,
  KEY_SEMICOLON = 59, /* ; */
  KEY_EQUAL = 61,     /* = */
  KEY_A = 65,
  KEY_B = 66,
  KEY_C = 67,
  KEY_D = 68,
  KEY_E = 69,
  KEY_F = 70,
  KEY_G = 71,
  KEY_H = 72,
  KEY_I = 73,
  KEY_J = 74,
  KEY_K = 75,
  KEY_L = 76,
  KEY_M = 77,
  KEY_N = 78,
  KEY_O = 79,
  KEY_P = 80,
  KEY_Q = 81,
  KEY_R = 82,
  KEY_S = 83,
  KEY_T = 84,
  KEY_U = 85,
  KEY_V = 86,
  KEY_W = 87,
  KEY_X = 88,
  KEY_Y = 89,
  KEY_Z = 90,
  KEY_LEFT_BRACKET = 91,  /* [ */
  KEY_BACKSLASH = 92,     /* \ */
  KEY_RIGHT_BRACKET = 93, /* ] */
  KEY_GRAVE_ACCENT = 96,  /* ` */
  KEY_WORLD_1 = 161,      /* non-US #1 */
  KEY_WORLD_2 = 162,      /* non-US #2 */

  /* Function keys */
  KEY_ESCAPE = 256,
  KEY_ENTER = 257,
  KEY_TAB = 258,
  KEY_BACKSPACE = 259,
  KEY_INSERT = 260,
  KEY_DELETE = 261,
  KEY_RIGHT = 262,
  KEY_LEFT = 263,
  KEY_DOWN = 264,
  KEY_UP = 265,
  KEY_PAGE_UP = 266,
  KEY_PAGE_DOWN = 267,
  KEY_HOME = 268,
  KEY_END = 269,
  KEY_CAPS_LOCK = 280,
  KEY_SCROLL_LOCK = 281,
  KEY_NUM_LOCK = 282,
  KEY_PRINT_SCREEN = 283,
  KEY_PAUSE = 284,
  KEY_F1 = 290,
  KEY_F2 = 291,
  KEY_F3 = 292,
  KEY_F4 = 293,
  KEY_F5 = 294,
  KEY_F6 = 295,
  KEY_F7 = 296,
  KEY_F8 = 297,
  KEY_F9 = 298,
  KEY_F10 = 299,
  KEY_F11 = 300,
  KEY_F12 = 301,
  KEY_F13 = 302,
  KEY_F14 = 303,
  KEY_F15 = 304,
  KEY_F16 = 305,
  KEY_F17 = 306,
  KEY_F18 = 307,
  KEY_F19 = 308,
  KEY_F20 = 309,
  KEY_F21 = 310,
  KEY_F22 = 311,
  KEY_F23 = 312,
  KEY_F24 = 313,
  KEY_F25 = 314,
  KEY_KP_0 = 320,
  KEY_KP_1 = 321,
  KEY_KP_2 = 322,
  KEY_KP_3 = 323,
  KEY_KP_4 = 324,
  KEY_KP_5 = 325,
  KEY_KP_6 = 326,
  KEY_KP_7 = 327,
  KEY_KP_8 = 328,
  KEY_KP_9 = 329,
  KEY_KP_DECIMAL = 330,
  KEY_KP_DIVIDE = 331,
  KEY_KP_MULTIPLY = 332,
  KEY_KP_SUBTRACT = 333,
  KEY_KP_ADD = 334,
  KEY_KP_ENTER = 335,
  KEY_KP_EQUAL = 336,
  KEY_LEFT_SHIFT = 340,
  KEY_LEFT_CONTROL = 341,
  KEY_LEFT_ALT = 342,
  KEY_LEFT_SUPER = 343,
  KEY_RIGHT_SHIFT = 344,
  KEY_RIGHT_CONTROL = 345,
  KEY_RIGHT_ALT = 346,
  KEY_RIGHT_SUPER = 347,
  KEY_MENU = 348,
  TOTAL = 349
};

enum class PressState : uint8_t
{
  RELEASED = 0,     // release button
  JUST_PRESSED = 1, // just pressed
  PRESSING = 2,     // pressing for a long time

  SHIFT = Utils::bit(2),
  CTRL = Utils::bit(3),
  ALT = Utils::bit(4),
  SUPER = Utils::bit(5),
  CAPS_LOCK = Utils::bit(6),
  NUM_LOCK = Utils::bit(7),
};

enum class InputAxis : int
{
  UNKNOWN = -1,
  MOUSE_CURSOR,
  GAMEPAD_LEFT_STICK,    // left stick
  GAMEPAD_RIGHT_STICK,   // Right stick
  GAMEPAD_LEFT_TRIGGER,  // L2
  GAMEPAD_RIGHT_TRIGGER, // R2
};

enum class InputDevice : uint32_t
{
  KEYBOARD_MOUSE = Utils::bit<uint32_t>(0),
  GAMEPAD_1 = Utils::bit<uint32_t>(1),
  GAMEPAD_2 = Utils::bit<uint32_t>(2),
  GAMEPAD_3 = Utils::bit<uint32_t>(3),
  PAMEPAD_4 = Utils::bit<uint32_t>(4),
  GAMEPAD_5 = Utils::bit<uint32_t>(5),
  GAMEPAD_6 = Utils::bit<uint32_t>(6),
  GAMEPAD_7 = Utils::bit<uint32_t>(7),
  PAMEPAD_8 = Utils::bit<uint32_t>(8),
  GAMEPAD_9 = Utils::bit<uint32_t>(9),
  GAMEPAD_10 = Utils::bit<uint32_t>(10),
  GAMEPAD_11 = Utils::bit<uint32_t>(11),
  PAMEPAD_12 = Utils::bit<uint32_t>(12),
  GAMEPAD_13 = Utils::bit<uint32_t>(13),
  GAMEPAD_14 = Utils::bit<uint32_t>(14),
  GAMEPAD_15 = Utils::bit<uint32_t>(15),
  PAMEPAD_16 = Utils::bit<uint32_t>(16),
  ALL = 0xFFFFFFFF
};

using CheckAxisStateFunc =
  std::function<std::optional<GameFramework::Vec3f>(InputDevice, InputAxis)>;
using CheckButtonStateFunc = std::function<PressState(InputDevice, InputButton)>;

constexpr inline PressState operator|(PressState s1, PressState s2)
{
  return static_cast<PressState>(static_cast<int>(s1) | static_cast<int>(s2));
}

constexpr inline int operator&(PressState s1, PressState s2)
{
  return static_cast<int>(s1) & static_cast<int>(s2);
}

constexpr inline InputDevice operator|(InputDevice d1, InputDevice d2)
{
  return static_cast<InputDevice>(static_cast<uint32_t>(d1) | static_cast<uint32_t>(d2));
}

constexpr inline uint32_t operator&(InputDevice d1, InputDevice d2)
{
  return static_cast<int>(d1) & static_cast<int>(d2);
}
} // namespace GameFramework
