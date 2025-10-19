#include <cassert>
#include <optional>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "IWindow.hpp"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

namespace
{

void OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods)
{
}

} // namespace

namespace Utils
{
struct GlfwWindow final : public IWindow
{
  GlfwWindow(const std::string & title, int width, int height);
  virtual ~GlfwWindow() override;

public:
  SurfaceDescriptor GetSurface() const noexcept override;
  std::pair<int, int> GetSize() const noexcept override;
  float GetAspectRatio() const noexcept override;
  void SetCursorHidden(bool hidden = true) override;
  bool IsCursorHidden() const noexcept override;
  void Close() override;
  bool ShouldClose() const noexcept override;
  void SetResizeCallback(ResizeCallback && callback) override;

public: // InputProducer
  virtual void BindInputQueue(GameFramework::InputQueue & queue) override;
  virtual void GenerateInputEvents() override;

public:
private:
  GLFWwindow * m_window = nullptr;
  std::optional<std::pair<double, double>> m_lastCursorPos;
  bool m_pressedKeys[GLFW_KEY_LAST]{false};
  ResizeCallback onResize = nullptr;
  std::vector<GameFramework::InputQueue *> m_boundInputs;

private:
  static void OnResizeCallback(GLFWwindow * window, int width, int height);
  static void OnCursorMoved(GLFWwindow * window, double xpos, double ypos);
  static void OnCursorEnterLeave(GLFWwindow * window, int entered);
  static void OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods);
  static void OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods);
  static void OnScroll(GLFWwindow * window, double xoffset, double yoffset);
};

GlfwWindow::GlfwWindow(const std::string & title, int width, int height)
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

void GlfwWindow::BindInputQueue(GameFramework::InputQueue & queue)
{
  m_boundInputs.push_back(&queue);
}

void GlfwWindow::GenerateInputEvents()
{
  auto generateEvent = [this](const GameFramework::GameInputEvent & evt)
  {
    for (auto * queue : m_boundInputs)
      queue->PushEvent(evt);
  };

  generateEvent(GameFramework::InputAction{1, true});
  //TODO: iterate over pressed keys and generate inputs
}

void GlfwWindow::OnResizeCallback(GLFWwindow * window, int width, int height)
{
  auto * wnd = reinterpret_cast<Utils::GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd && wnd->onResize)
    wnd->onResize(width, height);
}

void GlfwWindow::OnCursorMoved(GLFWwindow * window, double xpos, double ypos)
{
  auto * wnd = reinterpret_cast<Utils::GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    assert(wnd->m_lastCursorPos.has_value());

    //if (wnd->onMoveCursor)
    //  wnd->onMoveCursor(xpos, ypos, xpos - g_oldCursorPos.first, ypos - g_oldCursorPos.second);
    wnd->m_lastCursorPos = {xpos, ypos};
  }
}

void GlfwWindow::OnCursorEnterLeave(GLFWwindow * window, int entered)
{
  auto * wnd = reinterpret_cast<Utils::GlfwWindow *>(glfwGetWindowUserPointer(window));
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
  auto * wnd = reinterpret_cast<Utils::GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    if (action == GLFW_PRESS)
      wnd->m_pressedKeys[key] = true;
    else if (action == GLFW_RELEASE)
      wnd->m_pressedKeys[key] = false;
  }
}

void GlfwWindow::OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods)
{
  auto * wnd = reinterpret_cast<Utils::GlfwWindow *>(glfwGetWindowUserPointer(window));
  if (wnd)
  {
    if (action == GLFW_PRESS)
      wnd->m_pressedKeys[key] = true;
    else if (action == GLFW_RELEASE)
      wnd->m_pressedKeys[key] = false;
  }
}

void GlfwWindow::OnScroll(GLFWwindow * window, double xoffset, double yoffset)
{
}

std::unique_ptr<IWindow> NewWindow(const std::string & title, int width, int height)
{
  return std::make_unique<GlfwWindow>(title, width, height);
}

} // namespace Utils
