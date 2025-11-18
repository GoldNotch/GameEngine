#include "GlfwInstance.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <ranges>

#include <GLFW/glfw3.h>
#include <GlfwWindow.hpp>


namespace GlfwWindowsPlugin
{

GlfwInstance::GlfwInstance()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwSetErrorCallback(GlfwInstance::OnGlfwError);
  glfwSetJoystickCallback(GlfwInstance::OnJoystickConnected);

  // collect info about joysticks
  for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
  {
    if (!glfwJoystickPresent(jid))
      continue;
    const char * name = glfwGetJoystickName(jid);
    const char * guid = glfwGetJoystickGUID(jid);
    GameFramework::Log(GameFramework::LogMessageType::Info, "Joystick ", name, " connected");

    if (glfwJoystickIsGamepad(jid))
    {
      int res = glfwGetGamepadState(jid, &m_oldGamepadStates[jid]);
      if (res == GLFW_TRUE)
        OnJoystickConnected(jid, true);
    }
  }
}

GlfwInstance::~GlfwInstance()
{
  glfwTerminate();
}

void GlfwInstance::PollEvents()
{
  glfwPollEvents();
  if (m_connectedGamepadsCount > 0)
  {
    std::array<GLFWgamepadstate, JoystickCountLimit> newGamepadStates;
    for (size_t i = 0; i < m_connectedGamepadsCount; ++i)
    {
      int jid = m_connectedGamepads[i];
      int res = glfwGetGamepadState(jid, &newGamepadStates[jid]);
      if (res == GLFW_FALSE)
      {
        GameFramework::Log(GameFramework::LogMessageType::Warning, "Joystick - ", jid,
                           " has been disconnected unexpectedly");
        OnJoystickConnected(jid, false /*connected*/);
      }
    }
    std::swap(m_oldGamepadStates, m_gamepadStates);
    std::swap(m_gamepadStates, newGamepadStates);
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
  auto it = std::ranges::find(m_trackedWindows, wnd);
  if (it != m_trackedWindows.end())
  {
    std::swap(*it, m_trackedWindows.back());
    m_trackedWindows.pop_back();
  }
}

const GLFWgamepadstate & GlfwInstance::GetGamepadState(int jid) const & noexcept
{
  return m_gamepadStates[jid];
}

const GLFWgamepadstate & GlfwInstance::GetOldGamepadState(int jid) const & noexcept
{
  return m_oldGamepadStates[jid];
}

void GlfwInstance::OnJoystickConnected(int jid, bool connected)
{
  if (connected)
  {
    if (m_connectedGamepadsCount < JoystickCountLimit)
    {
      const size_t i = m_connectedGamepadsCount++;
      m_connectedGamepads[i] = jid;
    }
    else
    {
      assert(false);
    }
  }
  else
  {
    if (m_connectedGamepadsCount >= 1)
    {
      const size_t last = m_connectedGamepadsCount - 1;
      std::swap(m_connectedGamepads[jid], m_connectedGamepads[last]);
      m_connectedGamepadsCount--;
    }
    else
    {
      assert(false);
    }
  }

  // TODO: remove
  for (auto * wndPtr : m_trackedWindows)
  {
    assert(wndPtr);
    wndPtr->OnGamepadConnected(jid, connected);
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
