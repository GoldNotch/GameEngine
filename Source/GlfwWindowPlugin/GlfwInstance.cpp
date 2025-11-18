#include "GlfwInstance.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <ranges>

#include <GLFW/glfw3.h>
#include <GlfwInput.hpp>
#include <GlfwWindow.hpp>


namespace GlfwWindowsPlugin
{

GlfwInstance::GlfwInstance()
  : m_joystickStates(JoystickCountLimit)
  , m_oldJoystickStates(JoystickCountLimit)
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwSetErrorCallback(GlfwInstance::OnGlfwError);
  glfwSetJoystickCallback(GlfwInstance::OnJoystickConnected);

  // collect info about joysticks
  for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
  {
    if (glfwJoystickPresent(jid))
      OnJoystickConnected(jid, true);
  }
}

GlfwInstance::~GlfwInstance()
{
  glfwTerminate();
}

void GlfwInstance::PollEvents()
{
  glfwPollEvents();
  std::swap(m_oldJoystickStates, m_joystickStates);
  for (auto && [jid, description] : m_connectedJoysticks)
  {
    if (description.isGamepad)
    {
      GLFWgamepadstate state;
      int res = glfwGetGamepadState(jid, &state);
      if (res == GLFW_FALSE)
      {
        GameFramework::Log(GameFramework::LogMessageType::Warning, "Joystick - ", jid,
                           " has been disconnected unexpectedly");
        OnJoystickConnected(jid, false /*connected*/);
        continue;
      }
      m_joystickStates[jid].axes.assign(state.axes, state.axes + 6);
      m_joystickStates[jid].buttons.assign(state.buttons, state.buttons + 15);
    }
    else // generic joystick
    {
      int axesCount = 0;
      int buttonsCount = 0;
      int hatsCount = 0;
      const float * axes = glfwGetJoystickAxes(jid, &axesCount);
      const uint8_t * buttons = glfwGetJoystickButtons(jid, &buttonsCount);
      const uint8_t * hats = glfwGetJoystickHats(jid, &hatsCount);
      if (!axes || !buttons || !hats)
      {
        GameFramework::Log(GameFramework::LogMessageType::Warning, "Joystick - ", jid,
                           " has been disconnected unexpectedly");
        OnJoystickConnected(jid, false /*connected*/);
        continue;
      }

      m_joystickStates[jid].axes.assign(axes, axes + axesCount);
      m_joystickStates[jid].buttons.assign(buttons, buttons + buttonsCount);
      for (int i = 0; i < hatsCount; ++i)
      {
        // the order is important because it's assosiated with GLFW_GAMEPAD_BUTTON_DPAD_*
        m_joystickStates[jid].buttons.push_back(hats[i] & GLFW_HAT_UP);
        m_joystickStates[jid].buttons.push_back(hats[i] & GLFW_HAT_RIGHT);
        m_joystickStates[jid].buttons.push_back(hats[i] & GLFW_HAT_DOWN);
        m_joystickStates[jid].buttons.push_back(hats[i] & GLFW_HAT_LEFT);
      }
    }
  }
}

double GlfwInstance::GetTimestamp() const
{
  return glfwGetTime();
}

void GlfwInstance::TrackWindow(GlfwWindow * wnd)
{
  m_trackedWindows.push_back(wnd);
}

void GlfwInstance::UntrackWindow(GlfwWindow * wnd)
{
  auto range = std::ranges::remove(m_trackedWindows, wnd);
  m_trackedWindows.erase(range.begin(), range.end());
}

GameFramework::PressState GlfwInstance::CheckJoystickButtonState(
  int jid, GameFramework::InputButton button) const noexcept
{
  auto && state = m_joystickStates[jid];
  auto && oldState = m_oldJoystickStates[jid];

  int code = ConvertJoystickButton2Code(button);
  // if it was pressed later, then it's long pressing state,
  // if it's first press then it's JUST_PRESSED
  // if release, then RELEASED
  if (oldState.buttons[code] == GLFW_PRESS)
  {
    return state.buttons[code] == GLFW_PRESS ? GameFramework::PressState::PRESSING
                                             : GameFramework::PressState::RELEASED;
  }
  else // GLFW_RELEASE
  {
    return state.buttons[code] == GLFW_PRESS ? GameFramework::PressState::JUST_PRESSED
                                             : GameFramework::PressState::RELEASED;
  }
}

GameFramework::AxisValue GlfwInstance::CheckJoystickAxisState(
  int jid, GameFramework::InputAxis axis) const noexcept
{
  using namespace GameFramework;
  auto && state = m_joystickStates[jid];
  int idx = ConvertJoystickAxis2Code(axis);
  if (idx >= 0 && idx < state.axes.size())
  {
    return state.axes[idx];
  }
  else
  {
    return AxisNoValue;
  }
}

GameFramework::InputDeviceDescription GlfwInstance::GetDeviceDescription(
  GameFramework::InputDevice dev) const noexcept
{
  int jid = InputDevice2JoystickId(dev);
  auto it = m_connectedJoysticks.find(jid);
  if (it != m_connectedJoysticks.end())
    return it->second;
}

std::vector<int> GlfwInstance::GetConnectedJoysticks() const
{
  std::vector<int> result;
  result.reserve(m_connectedJoysticks.size());
  for (auto && [jid, _] : m_connectedJoysticks)
  {
    result.push_back(jid);
  }
  return result;
}

void GlfwInstance::OnJoystickConnected(int jid, bool connected)
{
  if (connected)
  {
    auto [it, inserted] =
      m_connectedJoysticks.insert({jid, GameFramework::InputDeviceDescription()});
    assert(inserted);

    GameFramework::InputDeviceDescription & description = it->second;
    description.name = glfwGetJoystickName(jid);
    description.guid = glfwGetJoystickGUID(jid);
    description.device = JoystickId2InputDevice(jid);
    description.isGamepad = glfwJoystickIsGamepad(jid);
    const float * axes = glfwGetJoystickAxes(jid, &description.axesCount);
    int buttonsCount;
    int hatsCount;
    const uint8_t * buttons = glfwGetJoystickButtons(jid, &buttonsCount);
    const uint8_t * hats = glfwGetJoystickHats(jid, &hatsCount);
    description.buttonsCount = buttonsCount + hatsCount * 4; // one hat it's a 4 buttons
    GameFramework::Log(GameFramework::LogMessageType::Info, "Joystick ", description.name,
                       " connected");
  }
  else
  {
    m_connectedJoysticks.erase(jid);
  }

  // TODO: remove
  for (auto * wndPtr : m_trackedWindows)
  {
    assert(wndPtr);
    wndPtr->OnJoystickConnected(jid, connected);
  }
}

void GlfwInstance::OnGlfwError(int code, const char * description)
{
  std::printf("Glfw Error %i - %s", code, description);
}

void GlfwInstance::OnJoystickConnected(int jid, int evt)
{
  bool connected = false;
  if (evt == GLFW_CONNECTED)
  {
    GameFramework::Log(GameFramework::LogMessageType::Info, "Gamepad - ", jid,
                       " - has been connected");
    connected = true;
  }
  else if (evt == GLFW_DISCONNECTED)
  {
    GameFramework::Log(GameFramework::LogMessageType::Info, "Gamepad - ", jid,
                       " - has been disconnected");
    connected = false;
  }
  GetGlfwInstance().OnJoystickConnected(jid, connected);
}


GlfwInstance & GetGlfwInstance()
{
  static GlfwInstance s_instance;
  return s_instance;
}

} // namespace GlfwWindowsPlugin
