#include <array>
#include <cassert>
#include <optional>
#include <stdexcept>

#include <GLFW/glfw3.h>
/// clang-format off
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>
/// clang-format on

#include <GlfwInput.hpp>
#include <GlfwInstance.hpp>
#include <GlfwWindow.hpp>
#include <Input/InputController.hpp>
using namespace GameFramework;

namespace GlfwWindowsPlugin
{

GlfwWindow::GlfwWindow(int id, const std::string & title, int width, int height)
  : m_id(id)
{
  GetGlfwInstance().TrackWindow(this);
  // Create GLFW window
  GLFWwindow * window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (window == NULL)
  {
    throw std::runtime_error("Failed to create GLFW window");
  }
  // set callback on resize
  glfwSetWindowUserPointer(window, this);
  glfwSetWindowSizeCallback(window, GlfwWindow::OnResizeCallback);
  glfwSetCursorPosCallback(window, GlfwWindow::OnCursorMoved);
  glfwSetCursorEnterCallback(window, GlfwWindow::OnCursorEnterLeave);
  glfwSetMouseButtonCallback(window, GlfwWindow::OnMouseButtonAction);
  glfwSetScrollCallback(window, GlfwWindow::OnScroll);
  glfwSetKeyCallback(window, GlfwWindow::OnKeyAction);
  m_window = window;
}

GlfwWindow::~GlfwWindow()
{
  GetGlfwInstance().UntrackWindow(this);
  glfwDestroyWindow(m_window);
}

SurfaceDescriptor GlfwWindow::GetSurface() const noexcept
{
#ifdef _WIN32
  return {glfwGetWin32Window(m_window), GetModuleHandle(nullptr)};
#elif defined(__linux__)
  return {reinterpret_cast<void *>(glfwGetX11Window(m_window)), glfwGetX11Display()};
#endif
}

std::pair<int, int> GlfwWindow::GetSize() const noexcept
{
  int width, height;
  glfwGetWindowSize(m_window, &width, &height);
  return {width, height};
}

float GlfwWindow::GetAspectRatio() const noexcept
{
  auto [width, height] = GetSize();
  return static_cast<float>(width) / static_cast<float>(height);
}

void GlfwWindow::SetCursorHidden(bool hidden)
{
  glfwSetInputMode(m_window, GLFW_CURSOR, hidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool GlfwWindow::IsCursorHidden() const noexcept
{
  return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void GlfwWindow::Close()
{
  glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

bool GlfwWindow::ShouldClose() const noexcept
{
  return glfwWindowShouldClose(m_window);
}

void GlfwWindow::SetResizeCallback(ResizeCallback && callback)
{
  onResize = std::move(callback);
}

InputBackend & GlfwWindow::GetInput() & noexcept
{
  return *this;
}

void GlfwWindow::OnJoystickConnected(int jid, bool connected)
{
  if (m_controller)
  {
    m_controller->OnNewInputDeviceConnected(JoystickId2InputDevice(jid), connected);
  }
}

void GlfwWindow::BindController(GameFramework::InputController * controller)
{
  m_controller = controller;
  if (m_controller)
  {
    m_controller->OnNewInputDeviceConnected(InputDevice::KEYBOARD, true);
    m_controller->OnNewInputDeviceConnected(InputDevice::MOUSE, true);
    auto && connectedJoysticks = GetGlfwInstance().GetConnectedJoysticks();
    for (int jid : connectedJoysticks)
      m_controller->OnNewInputDeviceConnected(JoystickId2InputDevice(jid), true);
  }
}

PressState GlfwWindow::CheckButtonState(InputDevice device, InputButton btn) const noexcept
{
  if (device == InputDevice::KEYBOARD || device == InputDevice::MOUSE)
  {
    return m_pressedButtons[static_cast<size_t>(btn)];
  }
  else if (!!(device & InputDevice::ANY_JOYSTICK))
  {
    // Get state of Gamepad button
    int jid = InputDevice2JoystickId(device);
    return GetGlfwInstance().CheckJoystickButtonState(jid, btn);
  }

  return PressState::RELEASED;
}

AxisValue GlfwWindow::CheckAxisState(InputDevice device, InputAxis axis) const noexcept
{
  if (device == InputDevice::MOUSE && m_curCursorPos)
  {
    return axis == InputAxis::MOUSE_CURSOR_X ? m_curCursorPos->x : m_curCursorPos->y;
  }
  else if (!!(device & InputDevice::ANY_JOYSTICK))
  {
    int jid = InputDevice2JoystickId(device);
    return GetGlfwInstance().CheckJoystickAxisState(jid, axis);
  }
  return AxisNoValue;
}

InputDeviceDescription GlfwWindow::GetInputDeviceDescription(InputDevice device) const noexcept
{
  return GetGlfwInstance().GetDeviceDescription(device);
}

void GlfwWindow::OnResizeCallback(GLFWwindow * window, int width, int height)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd && wnd->onResize)
    wnd->onResize(width, height);
}

void GlfwWindow::OnCursorMoved(GLFWwindow * window, double xpos, double ypos)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    wnd->m_curCursorPos = {static_cast<float>(xpos), static_cast<float>(ypos), 0.0f};
  }
}

void GlfwWindow::OnCursorEnterLeave(GLFWwindow * window, int entered)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    if (entered == GLFW_TRUE)
    {
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      wnd->m_curCursorPos = {static_cast<float>(x), static_cast<float>(y), 0.0f};
    }
    else
    {
      wnd->m_curCursorPos.reset();
    }
  }
}

void GlfwWindow::OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    size_t btnIdx = static_cast<size_t>(ConvertMouseButtonCode(key));
    wnd->m_pressedButtons[btnIdx] = ConvertPressState(action, mods);
  }
}

void GlfwWindow::OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    size_t btnIdx = static_cast<size_t>(ConvertMouseButtonCode(key));
    wnd->m_pressedButtons[btnIdx] = ConvertPressState(action, mods);
  }
}

void GlfwWindow::OnScroll(GLFWwindow * window, double xoffset, double yoffset)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    //wnd->PushInputEvent(AxisAction{});
  }
}

} // namespace GlfwWindowsPlugin
