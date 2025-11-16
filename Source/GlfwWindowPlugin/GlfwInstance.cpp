#include "GlfwInstance.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <ranges>

#include <GLFW/glfw3.h>
#include <GlfwWindow.hpp>

namespace
{

void OnGlfwError(int code, const char * description)
{
  std::printf("Glfw Error %i - %s", code, description);
}

} // namespace

namespace GlfwWindowsPlugin
{

GlfwInstance::GlfwInstance()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwSetErrorCallback(::OnGlfwError);
  glfwSetJoystickCallback(
    [](int jid, int evt)
    {
      if (evt == GLFW_CONNECTED)
      {
        GameFramework::Log(GameFramework::LogMessageType::Info, "Gamepad - ", jid,
                           " - has been connected");
      }
      else if (evt == GLFW_DISCONNECTED)
      {
        GameFramework::Log(GameFramework::LogMessageType::Info, "Gamepad - ", jid,
                           " - has been disconnected");
      }
      bool & oldValue = GetGlfwInstance().m_connectedGamepads[jid];
      bool newValue = (evt == GLFW_CONNECTED);
      if (oldValue != newValue)
      {
        for (auto * wndPtr : GetGlfwInstance().m_trackedWindows)
        {
          assert(wndPtr);
          wndPtr->OnGamepadConnected(jid, newValue);
        }
      }
      oldValue = newValue;
    });
}

GlfwInstance::~GlfwInstance()
{
  glfwTerminate();
}

void GlfwInstance::PollEvents()
{
  glfwPollEvents();
  for (size_t jid = 0; jid < GamepadsCountLimit; ++jid)
  {
    if (m_connectedGamepads[jid])
    {
      GLFWgamepadstate state;
      if (glfwGetGamepadState(static_cast<int>(jid), &state))
      {
        for (auto * wndPtr : m_trackedWindows)
        {
          assert(wndPtr);
          wndPtr->PollGamepadEvents(jid, state);
        }
      }
      else
      {
        assert(false);
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
  auto it = std::ranges::find(m_trackedWindows, wnd);
  if (it != m_trackedWindows.end())
  {
    std::swap(*it, m_trackedWindows.back());
    m_trackedWindows.pop_back();
  }
}


GlfwInstance & GetGlfwInstance()
{
  static GlfwInstance s_instance;
  return s_instance;
}

} // namespace GlfwWindowsPlugin
