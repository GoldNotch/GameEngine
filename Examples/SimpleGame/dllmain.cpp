#include <iostream>
#include <memory>
#include <vector>

#include <GameFramework.hpp>
using namespace GameFramework;

enum ActionCode
{
  MoveForward,
  MoveBackward,
  MoveLeft,
  MoveRight,
};

class SimpleGame : public GameFramework::GamePLugin
{
  float t = 0.0;

public:
  SimpleGame() = default;
  virtual ~SimpleGame() override = default;

  virtual std::string GetGameName() const override { return "SimpleGame"; }
  virtual std::vector<InputBinding> GetInputConfiguration() const override;

  ///
  void Tick(double deltaTime) override;

  /// Loop over game object and choose the way to render it
  void Render(GameFramework::IDrawTool & drawTool) override;

  void ProcessInputEvents() override;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const override;

private:
  std::vector<InputQueue *> m_listenedInput;
  std::vector<SignalsQueue *> m_boundSignalsQueue;
};

std::vector<InputBinding> SimpleGame::GetInputConfiguration() const
{
  std::vector<InputBinding> actions{
    {"MoveForward", ActionCode::MoveForward, "KeyW", ActionType::Continous},
    {"MoveBackward", ActionCode::MoveBackward, "KeyS", ActionType::Continous},
    {"MoveLeft", ActionCode::MoveLeft, "KeyA", ActionType::Continous},
    {"MoveRight", ActionCode::MoveRight, "KeyD", ActionType::Continous},
  };
  return actions;
}

std::vector<ProtoWindow> SimpleGame::GetOutputConfiguration() const
{
  std::vector<ProtoWindow> windows{{"SimpleGame", 800, 600}, {"TestWindow", 500, 500}};
  return windows;
}

void SimpleGame::Tick(double deltaTime)
{
  auto evt = ConsumeInputEvent();
  t += deltaTime;
  GenerateSignal(GameSignal::InvalidateRenderCache);
  //GameFramework::Log(GameFramework::Info, L"Tick: ", deltaTime, " FPS: ", 1000.0f / deltaTime);
}

void SimpleGame::Render(GameFramework::IDrawTool & drawTool)
{
  drawTool.SetClearColor({0.2f, 0.5f, (std::sin(t * 0.005f) + 1.0f) / 2.0f, 1.0f});

  float right = 0.5f + (std::sin(t * 0.002) + 1.0f) / 4.0f;
  float top = 0.5f + (std::sin(t * 0.002) + 1.0f) / 8.0f;
  drawTool.DrawRect(0.0f, top, right, 0.0f);
  drawTool.DrawRect(-0.5f, 0.0f, 0.0f, -0.2f);
}

void SimpleGame::ProcessInputEvents()
{
}

/// creates global game instance
GAME_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance()
{
  return std::make_unique<SimpleGame>();
}
