#include <iostream>
#include <memory>
#include <vector>

#include <GameFramework.hpp>

class SimpleGame
{
  float t = 0.0;

public:
  ///
  void Tick(float deltaTime)
  {
    t += deltaTime;
    GameFramework::Log(GameFramework::Info, L"Tick: ", deltaTime, " FPS: ", 1000.0f / deltaTime);
  }

  /// Loop over game object and choose the way to render it
  void Render(GameFramework::IDrawTool & drawTool)
  {
    drawTool.SetClearColor({0.2f, 0.5f, (std::sin(t * 0.005f) + 1.0f) / 2.0f, 1.0f});

    float right = 0.5f + (std::sin(t * 0.002) + 1.0f) / 4.0f;
    float top = 0.5f + (std::sin(t * 0.002) + 1.0f) / 8.0f;
    drawTool.DrawRect(0.0f, top, right, 0.0f);
    drawTool.DrawRect(-0.5f, 0.0f, 0.0f, -0.2f);
  }
};

static std::unique_ptr<SimpleGame> s_simpleGame;

enum ActionCode
{
  MoveForward,
  MoveBackward,
  MoveLeft,
  MoveRight,
};

/// Get name of the game
GAME_API std::string GetGameName()
{
  return "SimpleGame";
}

GAME_API std::vector<ProtoGameAction> GetInputConfiguration()
{
  std::vector<ProtoGameAction> actions{
    {"MoveForward", ActionCode::MoveForward, "KeyW"},
    {"MoveBackward", ActionCode::MoveBackward, "KeyS"},
    {"MoveLeft", ActionCode::MoveLeft, "KeyA"},
    {"MoveRight", ActionCode::MoveRight, "KeyD"},
  };
  return actions;
}

GAME_API std::vector<ProtoWindow> GetOutputConfiguration()
{
  std::vector<ProtoWindow> windows{{"SimpleGame", 800, 600}, {"TestWindow", 500, 500}};
  return windows;
}

/// creates global game instance
GAME_API void InitGame()
{
  s_simpleGame = std::make_unique<SimpleGame>();
}

/// Get name of the game
GAME_API void TerminateGame()
{
  s_simpleGame.reset();
}

/// Get name of the game
GAME_API void TickGame(float deltaTime)
{
  s_simpleGame->Tick(deltaTime);
}

GAME_API void RenderGame(GameFramework::IDrawTool & drawTool)
{
}

GAME_API void ProcessInput(const GameAction & action)
{
  //GameFramework::Log(GameFramework::LogMessageType::Info, action.name, L" action is processing");
  switch (action.code)
  {
    case ActionCode::MoveForward:
    case ActionCode::MoveBackward:
    case ActionCode::MoveLeft:
    case ActionCode::MoveRight:
      break;
  }
}
