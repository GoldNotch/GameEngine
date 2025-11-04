#include <iostream>
#include <memory>
#include <vector>

#include <GameFramework.hpp>
using namespace GameFramework;

enum Windows
{
  TEST_WINDOW1,
  TEST_WINDOW2
};

enum ActionCode
{
  Quit,
  MoveForward,
  MoveBackward,
  MoveLeft,
  MoveRight,
};

class SimpleGame : public GameFramework::GamePlugin
{
  float t = 0.0;

public:
  SimpleGame() = default;
  virtual ~SimpleGame() override = default;

  virtual std::string GetGameName() const override { return "SimpleGame"; }
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

std::vector<InputBinding> SimpleGame::GetInputConfiguration() const
{
  std::vector<InputBinding> actions{
    {"Quit", ActionCode::Quit, "KeyEscape", ActionType::Event},
    {"MoveForward", ActionCode::MoveForward, "KeyW", ActionType::Continous},
    {"MoveBackward", ActionCode::MoveBackward, "KeyS", ActionType::Continous},
    {"MoveLeft", ActionCode::MoveLeft, "KeyA", ActionType::Continous},
    {"MoveRight", ActionCode::MoveRight, "KeyD", ActionType::Continous},
  };
  return actions;
}

std::vector<ProtoWindow> SimpleGame::GetOutputConfiguration() const
{
  std::vector<ProtoWindow> windows{{TEST_WINDOW1, "SimpleGame", 800, 600},
                                   {TEST_WINDOW2, "TestWindow", 500, 500}};
  return windows;
}

void SimpleGame::ProcessInput()
{
  auto evt = ConsumeInputEvent();
  if (evt.has_value())
  {
    std::visit(Utils::overloaded{[this](const EventAction & evt)
                                 {
                                   if (evt.code == ActionCode::Quit)
                                     GenerateSignal(GameSignal::Quit);
                                 },
                                 [](const ContinousAction & action) {

                                 },
                                 [](const AxisAction & action) {
                                 }},
               *evt);
  }
}

void SimpleGame::Tick(double deltaTime)
{
  ProcessInput();
  t += static_cast<float>(deltaTime);
  GenerateSignal(GameSignal::InvalidateRenderCache);
  GameFramework::Log(GameFramework::LogMessageType::Info, "Tick: ", deltaTime * 1000.0,
                     " FPS: ", 1.0 / deltaTime);
}

void SimpleGame::Render(GameFramework::IDevice & device)
{
  auto scene = device.AcquireScene2D();
  if (scene)
  {
    if (device.GetOwnerId() == TEST_WINDOW1)
    {
      scene->AddBackground({0.2f, 0.5f, (std::sin(t * 0.5f) + 1.0f) / 2.0f, 1.0f});
      float right = 0.5f + (std::sin(t * 0.002f) + 1.0f) / 4.0f;
      float top = 0.5f + (std::sin(t * 0.002f) + 1.0f) / 8.0f;
      scene->AddRect(0.0f, top, right, 0.0f);
    }
    else
    {
      scene->AddBackground({0.2f, 0.5f, 0.7f, 1.0f});
      scene->AddRect(-0.5f, 0.0f, 0.0f, -0.2f);
    }
  }
}

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance(
  const GameFramework::IPluginLoader & loader)
{
  return std::make_unique<SimpleGame>();
}
