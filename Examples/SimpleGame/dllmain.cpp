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

class SimpleGame : public GameFramework::BaseGame
{
  float t = 0.0;

public:
  SimpleGame() = default;
  virtual ~SimpleGame() override = default;

  virtual std::string GetGameName() const override { return "SimpleGame"; }
  virtual std::vector<ProtoGameAction> GetInputConfiguration() const override;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const override;

  ///
  void Tick(float deltaTime) override;

  /// Loop over game object and choose the way to render it
  void Render(GameFramework::IDrawTool & drawTool) override;
};

std::vector<ProtoGameAction> SimpleGame::GetInputConfiguration() const
{
  std::vector<ProtoGameAction> actions{
    {"MoveForward", ActionCode::MoveForward, "KeyW"},
    {"MoveBackward", ActionCode::MoveBackward, "KeyS"},
    {"MoveLeft", ActionCode::MoveLeft, "KeyA"},
    {"MoveRight", ActionCode::MoveRight, "KeyD"},
  };
  return actions;
}

std::vector<ProtoWindow> SimpleGame::GetOutputConfiguration() const
{
  std::vector<ProtoWindow> windows{{"SimpleGame", 800, 600}, {"TestWindow", 500, 500}};
  return windows;
}

void SimpleGame::Tick(float deltaTime)
{
  t += deltaTime;
  PushSignal(InvalidateRenderCacheSignal{});
  GameFramework::Log(GameFramework::Info, L"Tick: ", deltaTime, " FPS: ", 1000.0f / deltaTime);
}

void SimpleGame::Render(GameFramework::IDrawTool & drawTool)
{
  drawTool.SetClearColor({0.2f, 0.5f, (std::sin(t * 0.005f) + 1.0f) / 2.0f, 1.0f});

  float right = 0.5f + (std::sin(t * 0.002) + 1.0f) / 4.0f;
  float top = 0.5f + (std::sin(t * 0.002) + 1.0f) / 8.0f;
  drawTool.DrawRect(0.0f, top, right, 0.0f);
  drawTool.DrawRect(-0.5f, 0.0f, 0.0f, -0.2f);
}

/// creates global game instance
GAME_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance()
{
  return std::make_unique<SimpleGame>();
}
