#pragma once
#include <array>
#include <vector>

#include <GLFW/glfw3.h>

namespace GlfwWindowsPlugin
{
/// @brief Max count of gamepads
static constexpr size_t GamepadsCountLimit = GLFW_JOYSTICK_LAST + 1;

struct GlfwWindow;

struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
  double GetTimestamp() const;
  void TrackWindow(GlfwWindow * wnd);
  void UntrackWindow(GlfwWindow * wnd);

private:
  std::vector<GlfwWindow *> m_trackedWindows;
  std::array<bool, GamepadsCountLimit> m_connectedGamepads{false};
};

GlfwInstance & GetGlfwInstance();

} // namespace GlfwWindowsPlugin
