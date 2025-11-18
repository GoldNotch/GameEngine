#pragma once
#include <array>
#include <unordered_map>
#include <vector>

#include <GLFW/glfw3.h>
#include <Input/InputDevice.hpp>

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
  /// @brief poll glfw events and collect states from joysticks
  void PollEvents();
  double GetTimestamp() const;

  /// @brief store pointer on window. Used to say the window that new device is connected
  /// @param wnd - window
  void TrackWindow(GlfwWindow * wnd);

  /// @brief delete pointer on window
  /// @param wnd  - window
  void UntrackWindow(GlfwWindow * wnd);

  GameFramework::PressState CheckJoystickButtonState(
    int jid, GameFramework::InputButton button) const noexcept;

  GameFramework::AxisValue CheckJoystickAxisState(int jid,
                                                  GameFramework::InputAxis axis) const noexcept;

  GameFramework::InputDeviceDescription GetDeviceDescription(
    GameFramework::InputDevice dev) const noexcept;

  std::vector<int> GetConnectedJoysticks() const;

private:
  struct JoystickState
  {
    std::vector<float> axes;
    std::vector<uint8_t> buttons;
  };

  std::vector<GlfwWindow *> m_trackedWindows;

  std::unordered_map<int, GameFramework::InputDeviceDescription> m_connectedJoysticks;

  // joystick's states
  std::vector<JoystickState> m_joystickStates;
  std::vector<JoystickState> m_oldJoystickStates;

private: //Glfw callbacks
  static void OnGlfwError(int code, const char * description);
  static void OnJoystickConnected(int jid, int evt);

private:
  void OnJoystickConnected(int jid, bool connected);
};

GlfwInstance & GetGlfwInstance();

} // namespace GlfwWindowsPlugin
