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

#include <Input/InputController.hpp>

#include "GlfwInput.hpp"
#include "GlfwWindow.hpp"


namespace GlfwWindowsPlugin
{

struct GlfwWindow final : public GameFramework::IWindow
{
  GlfwWindow(const std::string & title, int width, int height);
  virtual ~GlfwWindow() override;

public:
  GameFramework::SurfaceDescriptor GetSurface() const noexcept override;
  std::pair<int, int> GetSize() const noexcept override;
  float GetAspectRatio() const noexcept override;
  void SetCursorHidden(bool hidden = true) override;
  bool IsCursorHidden() const noexcept override;
  void Close() override;
  bool ShouldClose() const noexcept override;
  void SetResizeCallback(ResizeCallback && callback) override;
  virtual GameFramework::InputController & GetInputController() & noexcept override;

private:
  GameFramework::InputControllerUPtr m_inputController;
  GLFWwindow * m_window = nullptr;
  std::optional<std::pair<double, double>> m_lastCursorPos;
  ResizeCallback onResize = nullptr;


private:
  static void OnResizeCallback(GLFWwindow * window, int width, int height);
  static void OnCursorMoved(GLFWwindow * window, double xpos, double ypos);
  static void OnCursorEnterLeave(GLFWwindow * window, int entered);
  static void OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods);
  static void OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods);
  static void OnScroll(GLFWwindow * window, double xoffset, double yoffset);
};

GlfwWindow::GlfwWindow(const std::string & title, int width, int height)
  : m_inputController(GameFramework::CreateInputController())
{
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
  glfwDestroyWindow(m_window);
}

GameFramework::SurfaceDescriptor GlfwWindow::GetSurface() const noexcept
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

GameFramework::InputController & GlfwWindow::GetInputController() & noexcept
{
  return *m_inputController;
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
    assert(wnd->m_lastCursorPos.has_value());
    //wnd->PushInputEvent(GameFramework::AxisAction{});
    wnd->m_lastCursorPos = {xpos, ypos};
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
      wnd->m_lastCursorPos = {x, y};
    }
    else
    {
      wnd->m_lastCursorPos.reset();
    }
  }
}

void GlfwWindow::OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
    wnd->m_inputController->OnButtonAction(ConvertKeyboardButtonCode(key),
                                           ConvertPressState(action, mods));
}

void GlfwWindow::OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
    wnd->m_inputController->OnButtonAction(ConvertMouseButtonCode(key),
                                           ConvertPressState(action, mods));
}

void GlfwWindow::OnScroll(GLFWwindow * window, double xoffset, double yoffset)
{
  auto * wnd = reinterpret_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    //wnd->PushInputEvent(GameFramework::AxisAction{});
  }
}

GameFramework::WindowUPtr NewWindowImpl(const std::string & title, int width, int height)
{
  return std::make_unique<GlfwWindow>(title, width, height);
}

} // namespace GlfwWindowsPlugin
