#pragma once

#include "GameFramework_def.h"

#include <array>
#include <stdexcept>
#include <string>

namespace GameFramework
{
/// public functions, which
enum GAME_FRAMEWORK_API LogMessageType
{
  Info,
  Warning,
  Error,
  Debug
};

/// Log message during game is running
GAME_FRAMEWORK_API void LogImpl(LogMessageType type, const std::wstring & message);

template<typename... Args>
inline void Log(LogMessageType type, const wchar_t * fmt, Args &&... args)
{
  thread_local std::wstring buffer(1024, L'\0');
  int bufSize = std::swprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
  if (bufSize < 0)
    throw std::runtime_error("Failed to log message");
  if (bufSize > buffer.size())
    buffer.resize(bufSize);
  std::swprintf(buffer.data(), fmt, std::forward<Args>(args)...);
  LogImpl(type, buffer);
}

/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct GAME_FRAMEWORK_API IDrawTool
{
  virtual ~IDrawTool() = default;
  virtual void SetClearColor(const std::array<float, 4> & color) = 0;
  virtual void Flush() = 0;
  //virtual void DrawCube(/*transform*/) = 0;
  //virtual void DrawSphere(/*transform*/) = 0;
};

struct GAME_FRAMEWORK_API IGame
{
  virtual ~IGame() = default;
  /// process one tick of the game
  virtual void Tick(float deltaTime) = 0;
  /// Loop over game object and choose the way to render it
  virtual void Render(IDrawTool & drawTool) = 0;
};

} // namespace GameFramework


/// It's functions which should be implemented in dll for Game Framework detected it as Game
#ifndef GAME_FRAMEWORK_BUILD

#ifdef __cplusplus
extern "C"
{
#endif

  /// creates global game instance
  GAME_API void InitGame();
  /// Get name of the game
  GAME_API void TerminateGame();
  /// creates global game instance
  GAME_API GameFramework::IGame * GetGameHandle();
  /// Get name of the game
  GAME_API const char * GetGameName();

#ifdef __cplusplus
}
#endif

#endif
