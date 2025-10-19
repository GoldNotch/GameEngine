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
  virtual std::vector<ProtoGameAction> GetInputConfiguration() const override;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const override;

  ///
  void Tick(float deltaTime) override;

  /// Loop over game object and choose the way to render it
  void Render(GameFramework::IDrawTool & drawTool) override;

public: // InputConsumer
  virtual void ListenInputQueue(GameFramework::InputQueue & queue) override;
  virtual std::optional<GameInputEvent> ConsumeEvent() override;

public: //SignalsProducer
  virtual void BindSignalsQueue(GameFramework::SignalsQueue & queue) override;

  virtual void GenerateSignal(const GameSignal & signal) override;

private:
  std::vector<InputQueue *> m_listenedInput;
  std::vector<SignalsQueue *> m_boundSignalsQueue;
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
  auto evt = ConsumeEvent();
  t += deltaTime;
  GenerateSignal(GameSignal::InvalidateRenderCache);
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

void SimpleGame::ListenInputQueue(GameFramework::InputQueue & queue)
{
  m_listenedInput.push_back(&queue);
}

std::optional<GameInputEvent> SimpleGame::ConsumeEvent()
{
  // очень спорно
  for (auto * queue : m_listenedInput)
  {
    auto evt = queue->PopEvent();
    if (evt.has_value())
      return evt;
  }
  return std::nullopt;
}

void SimpleGame::BindSignalsQueue(GameFramework::SignalsQueue & queue)
{
  m_boundSignalsQueue.push_back(&queue);
}

void SimpleGame::GenerateSignal(const GameSignal & signal)
{
  for (auto * queue : m_boundSignalsQueue)
    queue->PushSignal(signal);
}

/// creates global game instance
GAME_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance()
{
  return std::make_unique<SimpleGame>();
}
