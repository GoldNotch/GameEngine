#pragma once

#include <unordered_map>

#include <GameFramework.hpp>
#include <GLFW/glfw3.h>

namespace GlfwWindowsPlugin
{

struct GlfwWindow final : public GameFramework::IWindow,
                          public GameFramework::InputBackend
{
  GlfwWindow(int id, const std::string & title, int width, int height);
  virtual ~GlfwWindow() override;

public:
  virtual int GetId() const noexcept override { return m_id; }
  virtual std::string GetTitle() const noexcept override { return m_title; };
  GameFramework::SurfaceDescriptor GetSurface() const noexcept override;
  std::pair<int, int> GetSize() const noexcept override;
  float GetAspectRatio() const noexcept override;
  void SetCursorHidden(bool hidden = true) override;
  bool IsCursorHidden() const noexcept override;
  void Close() override;
  bool ShouldClose() const noexcept override;
  void SetResizeCallback(ResizeCallback && callback) override;
  virtual InputBackend & GetInput() & noexcept override;

public: // InputBackend
  virtual void BindController(GameFramework::InputController * controller) override;
  virtual GameFramework::PressState CheckButtonState(
    GameFramework::InputDevice device, GameFramework::InputButton btn) const noexcept override;
  virtual GameFramework::AxisValue CheckAxisState(
    GameFramework::InputDevice device, GameFramework::InputAxis axis) const noexcept override;
  virtual GameFramework::InputDeviceDescription GetInputDeviceDescription(
    GameFramework::InputDevice device) const noexcept override;

public: // Internal API
  /// @brief called when gamepad has been connected to machine
  /// @param jid - id of gamepad
  /// @param connected - true if connected, false otherwise
  void OnJoystickConnected(int jid, bool connected);

private:
  int m_id;                                                ///< user-defined id of window
  std::string m_title;                                     ///< title of window
  GLFWwindow * m_window = nullptr;                         ///< GLFW handle
  GameFramework::InputController * m_controller = nullptr; ///< doesn't own

  std::optional<GameFramework::Vec3f> m_curCursorPos; ///< position of cursor
  /// @brief state of each button
  std::array<GameFramework::PressState, static_cast<size_t>(GameFramework::InputButton::TOTAL)>
    m_pressedButtons;

  ResizeCallback onResize = nullptr;

private:
  static void OnResizeCallback(GLFWwindow * window, int width, int height);
  static void OnCursorMoved(GLFWwindow * window, double xpos, double ypos);
  static void OnCursorEnterLeave(GLFWwindow * window, int entered);
  static void OnKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods);
  static void OnMouseButtonAction(GLFWwindow * window, int key, int action, int mods);
  static void OnScroll(GLFWwindow * window, double xoffset, double yoffset);
};
} // namespace GlfwWindowsPlugin
