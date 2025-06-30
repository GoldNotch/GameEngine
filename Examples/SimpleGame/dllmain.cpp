#include <iostream>
#include <memory>

#include <GameFramework.hpp>

class SimpleGame : public GameFramework::IGame
{
public:
  virtual ~SimpleGame() = default;
  ///
  virtual void Tick(float deltaTime) override
  {
    std::cout << "Tick: " << deltaTime << " FPS: " << 1000.0f / deltaTime << std::endl;
  }

  /// Loop over game object and choose the way to render it
  virtual void Render(GameFramework::IDrawTool & drawTool) override {}
};

static std::unique_ptr<SimpleGame> s_simpleGame;

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

/// creates global game instance
GAME_API GameFramework::IGame * GetGameHandle()
{
  return s_simpleGame.get();
}

/// Get name of the game
GAME_API const char * GetGameName()
{
  return "SimpleGame";
}
