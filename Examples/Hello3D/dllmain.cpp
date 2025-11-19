#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include <GameFramework.hpp>

#include "FPSCamera.hpp"

using namespace GameFramework;

enum ActionCode
{
  Quit,
  MoveForward,
  MoveBackward,
  MoveLeft,
  MoveRight,
  RotateCamera
};

class Hello3D : public GameFramework::GamePlugin
{
  float t = 0.0;
  FPSCamera m_camera{Vec3f{0.0, -1.0, 0.0}};

public:
  Hello3D() { m_camera.SetPosition({0, 0, -10}); }
  virtual ~Hello3D() override = default;

  virtual std::string GetGameName() const override { return "Hello3D"; }
  virtual std::vector<InputBinding> GetInputConfiguration() const override;

  ///
  virtual void Tick(double deltaTime) override;

  /// Loop over game object and choose the way to render it
  virtual void Render(GameFramework::IDevice & device) override;

  virtual std::vector<ProtoWindow> GetOutputConfiguration() const override;

private:
  void ProcessInput();

private:
  std::vector<InputQueue *> m_listenedInput;
  std::vector<SignalsQueue *> m_boundSignalsQueue;
};

std::vector<InputBinding> Hello3D::GetInputConfiguration() const
{
  // clang-format off
  std::vector<InputBinding>
    actions{{"Quit", ActionCode::Quit, "KeyEscape", ActionType::Event},
            {"MoveForward", ActionCode::MoveForward, "KeyW", ActionType::Continous},
            {"MoveBackward", ActionCode::MoveBackward, "KeyS", ActionType::Continous},
            {"MoveLeft", ActionCode::MoveLeft, "KeyA",  ActionType::Continous},
            {"MoveRight", ActionCode::MoveRight, "KeyD",  ActionType::Continous},
            {"RotateCamera", ActionCode::RotateCamera, "MouseCursorX+MouseCursorY;GamepadLeftStickX+GamepadLeftStickY", ActionType::Axis}};
  // clang=format on
  return actions;
}

std::vector<ProtoWindow> Hello3D::GetOutputConfiguration() const
{
  std::vector<ProtoWindow> windows{{1, "Hello3D", 800, 600}};
  return windows;
}

void Hello3D::ProcessInput()
{
  auto evt = ConsumeInputEvent();
  while (evt.has_value())
  {
    std::visit(Utils::overloaded{[this](const EventAction & evt)
                                 {
                                   if (evt.code == ActionCode::Quit)
                                     GenerateSignal(GameSignal::Quit);
                                 },
                                 [this](const ContinousAction & action) {
                                    if (action.code == ActionCode::MoveForward)
                                        m_camera.MoveCamera(m_camera.GetFrontVector());
                                    if (action.code == ActionCode::MoveBackward)
                                        m_camera.MoveCamera(-m_camera.GetFrontVector());
                                    if (action.code == ActionCode::MoveLeft)
                                        m_camera.MoveCamera(m_camera.GetRightVector());
                                    if (action.code == ActionCode::MoveRight)
                                        m_camera.MoveCamera(-m_camera.GetRightVector());
                                 },
                                 [this](const AxisAction & axis) {
                                       if (axis.code == ActionCode::RotateCamera)
                                       {
                                           if (axis.device == InputDevice::MOUSE)
                                               m_camera.RotateCamera({ -axis.deltaValue[0], -axis.deltaValue[1] });
                                           if (!!(axis.device & InputDevice::ANY_JOYSTICK))
                                               m_camera.RotateCamera({ -axis.axisValue[0], -axis.axisValue[1] });
                                       }
                                 }},
               *evt);
    evt = ConsumeInputEvent();
  }
}

void Hello3D::Tick(double deltaTime)
{
  ProcessInput();
  t += static_cast<float>(deltaTime);
  GenerateSignal(GameSignal::InvalidateRenderCache);
  GameFramework::Log(GameFramework::LogMessageType::Info, "Tick: ", deltaTime * 1000.0,
                     " FPS: ", 1.0 / deltaTime);
}

void Hello3D::Render(GameFramework::IDevice & device)
{
  if (auto scene = device.AcquireScene2D())
  {
    scene->SetBackground({0.2f, 0.5f, (std::sin(t * 0.5f) + 1.0f) / 2.0f});
  }
  if (auto scene = device.AcquireScene3D())
  {
    Camera cam;
    cam.SetPlacement(m_camera.GetPosition(), m_camera.GetFrontVector());
    cam.SetPerspectiveSettings(
      PerspectiveSettings{45.0f, device.GetAspectRatio(), {0.1f, 100.0f}});
    scene->SetCamera(cam);
    scene->AddCube(Cube());
    scene->AddCube(Cube(Vec3f{3, -3.0, 0.0f}));
  }
}

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance(
  const GameFramework::IPluginLoader & loader)
{
  return std::make_unique<Hello3D>();
}
