#pragma once
#include <GameFramework_def.h>

#include <array>
#include <memory>

namespace GameFramework
{
/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct IDrawTool
{
  virtual ~IDrawTool() = default;
  virtual void SetClearColor(const std::array<float, 4> & color) = 0;
  virtual void Flush() = 0;
  //virtual void DrawCube(/*transform*/) = 0;
  //virtual void DrawSphere(/*transform*/) = 0;
};

struct IGame
{
  virtual ~IGame() = default;
  /// process one tick of the game
  virtual void Tick(float deltaTime) = 0;
  /// Loop over game object and choose the way to render it
  virtual void Render(IDrawTool & drawTool) = 0;
};
} // namespace GameFramework

extern "C"
{
  enum GAME_API LogMessageType
  {
    Info,
    Warning,
    Error,
    Debug
  };

  /// creates global game instance
  GAME_API void InitGame();
  /// Get name of the game
  GAME_API void TerminateGame();
  /// creates global game instance
  GAME_API GameFramework::IGame * GetGameHandle();
  /// Get name of the game
  GAME_API const char * GetGameName();
}
