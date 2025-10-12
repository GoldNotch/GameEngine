#pragma once

#include "GameFramework_def.h"

#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

#include <GameInstance/GameAPI.h>

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

} // namespace GameFramework
