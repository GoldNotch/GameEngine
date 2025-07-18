#pragma once

#include "GameFramework_def.h"

#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

namespace GameFramework
{

enum GAME_FRAMEWORK_API LogMessageType
{
  Info,
  Warning,
  Error,
  Debug
};

namespace detail
{
/// Log message during game is running
GAME_FRAMEWORK_API void LogImpl(LogMessageType type, std::wstring && message);
} // namespace detail

template<typename... Args>
inline void Log(LogMessageType type, Args &&... args)
{
  thread_local std::wostringstream stream;
  stream.str(L"");
  stream.clear();
  (stream << ... << std::forward<Args>(args));
  detail::LogImpl(type, stream.str());
}

using ResourceHandle = void *;

struct vec3
{
  float x;
  float y;
  float z;
};

GAME_FRAMEWORK_API ResourceHandle LoadStaticMesh(const char * path);

struct GAME_FRAMEWORK_API IDrawFuncs2D
{
  virtual ~IDrawFuncs2D() = default;
  virtual void DrawRect(float left, float top, float right, float bottom) = 0;
};

/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct GAME_FRAMEWORK_API IDrawTool : public IDrawFuncs2D
{
  virtual ~IDrawTool() = default;
  virtual void SetClearColor(const std::array<float, 4> & color) = 0;
  //virtual void SetViewport(float left, float top, uint32_t width, uint32_t height) = 0;
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
