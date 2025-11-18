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

void GlfwWindow::OnGamepadConnected(int jid, bool connected)
{
  if (m_controller)
  {
    m_controller->OnNewInputDeviceConnected(GamepadId2InputDevice(jid), connected);
  }
}

void GlfwWindow::BindController(GameFramework::InputController * controller)
{
  m_controller = controller;
  if (m_controller)
  {
    m_controller->OnNewInputDeviceConnected(InputDevice::KEYBOARD_MOUSE, true);
  }
}

PressState GlfwWindow::CheckButtonState(InputDevice device, InputButton btn) const noexcept
{
  if (device == InputDevice::KEYBOARD_MOUSE)
  {
    return m_pressedButtons[static_cast<size_t>(btn)];
  }
  else /*if (device == InputDevice::GAMEPAD_i)*/
  {
    // Get state of Gamepad button
    int jid = InputDevice2GamepadId(device);
    auto && state = GetGlfwInstance().GetGamepadState(jid);
    auto&& oldState = GetGlfwInstance().GetOldGamepadState(jid);

    int code = ConvertGamepadButton2Code(btn);
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
}

std::optional<Vec3f> GlfwWindow::CheckAxisState(InputDevice device, InputAxis axis) const noexcept
{
  if (device == InputDevice::KEYBOARD_MOUSE && axis == InputAxis::MOUSE_CURSOR)
  {
    return m_curCursorPos;
  }
  else /*if (device == InputDevice::ANY_GAMEPAD)*/
  {
    int jid = InputDevice2GamepadId(device);
    auto && state = GetGlfwInstance().GetGamepadState(jid);

    constexpr int lx = GLFW_GAMEPAD_AXIS_LEFT_X;
    constexpr int ly = GLFW_GAMEPAD_AXIS_LEFT_Y;
    constexpr int rx = GLFW_GAMEPAD_AXIS_RIGHT_X;
    constexpr int ry = GLFW_GAMEPAD_AXIS_RIGHT_Y;
    constexpr int l2 = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
    constexpr int r2 = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
    switch (axis)
    {
      case InputAxis::GAMEPAD_LEFT_STICK:
        return Vec3f{state.axes[lx], state.axes[ly], 0.0f};
      case InputAxis::GAMEPAD_RIGHT_STICK:
        return Vec3f{state.axes[rx], state.axes[ry], 0.0f};
      case InputAxis::GAMEPAD_LEFT_TRIGGER:
        return Vec3f{state.axes[l2], 0.0f, 0.0f};
      case InputAxis::GAMEPAD_RIGHT_TRIGGER:
        return Vec3f{state.axes[r2], 0.0f, 0.0f};
      default:
        return std::nullopt;
    }
  }
  return std::nullopt;
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
