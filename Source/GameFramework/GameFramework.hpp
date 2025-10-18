#pragma once

#include "GameFramework_def.h"

#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

#include <GameAPI.hpp>

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

using GetGameNameFunc = decltype(GetGameName);
using GetInputConfigurationFunc = decltype(GetInputConfiguration);
using GetOutputConfigurationFunc = decltype(GetOutputConfiguration);
using InitGameFunc = decltype(InitGame);
using TickGameFunc = decltype(TickGame);
using RenderGameFunc = decltype(RenderGame);
using ProcessInputFunc = decltype(ProcessInput);
using TerminateGameFunc = decltype(TerminateGame);

} // namespace GameFramework
