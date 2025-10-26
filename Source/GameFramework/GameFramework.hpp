#pragma once

#include "GameFramework_def.h"

#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

#include <PluginInterfaces/DrawToolPlugin.hpp>
#include <PluginInterfaces/GamePlugin.hpp>
#include <PluginInterfaces/WindowPlugin.hpp>

namespace GameFramework
{

enum class LogMessageType : uint8_t
{
  Info,
  Warning,
  Error,
  Debug
};

namespace details
{
/// Log message during game is running
GAME_FRAMEWORK_API void LogImpl(LogMessageType type, std::wstring && message);
} // namespace details

template<typename... Args>
inline void Log(LogMessageType type, Args &&... args)
{
  thread_local std::wostringstream stream;
  auto writeToStream = [](std::wostringstream & stream, auto && value)
  {
    using T = decltype(value);
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
    {
      // Convert std::string to std::wstring
      stream << std::wstring(value.begin(), value.end());
    }
    else if constexpr (std::is_same_v<std::decay_t<T>, const char *>)
    {
      // Convert const char* to std::wstring
      stream << std::wstring(value, value + std::strlen(value));
    }
    else
    {
      // Normal case (includes std::wstring, numbers, etc.)
      stream << std::forward<T>(value);
    }
  };

  stream.str(L"");
  stream.clear();
  (writeToStream(stream, std::forward<Args>(args)), ...);
  details::LogImpl(type, stream.str());
}

} // namespace GameFramework
