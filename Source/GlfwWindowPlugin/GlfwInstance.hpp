#pragma once
#include <array>
#include <vector>

#include <GLFW/glfw3.h>

namespace GlfwWindowsPlugin
{
/// @brief Max count of gamepads
static constexpr size_t JoystickCountLimit = GLFW_JOYSTICK_LAST + 1;
static constexpr int UNKNOWN_JOYSITCK = -1;

struct GlfwWindow;

struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
  double GetTimestamp() const;
  void TrackWindow(GlfwWindow * wnd);
  void UntrackWindow(GlfwWindow * wnd);
  const GLFWgamepadstate & GetGamepadState(int jid) const & noexcept;
  const GLFWgamepadstate & GetOldGamepadState(int jid) const & noexcept;

private:
  std::vector<GlfwWindow *> m_trackedWindows;

  size_t m_connectedGamepadsCount = 0;
  std::array<int, JoystickCountLimit> m_connectedGamepads{ UNKNOWN_JOYSITCK };
  std::array<GLFWgamepadstate, JoystickCountLimit> m_gamepadStates;
  std::array<GLFWgamepadstate, JoystickCountLimit> m_oldGamepadStates;

private: //Glfw callbacks
  static void OnGlfwError(int code, const char * description);
  static void OnJoystickConnected(int jid, int evt);

private:
  void OnJoystickConnected(int jid, bool connected);
};

GlfwInstance & GetGlfwInstance();

} // namespace GlfwWindowsPlugin
