#include "GameFramework.hpp"

#include <iostream>

namespace GameFramework::details
{

GAME_FRAMEWORK_API void LogImpl(LogMessageType type, std::wstring && message)
{
  const wchar_t * prefix = nullptr;
  switch (type)
  {
    case LogMessageType::Info:
      prefix = L"INFO: ";
      break;
    case LogMessageType::Warning:
      prefix = L"WARN: ";
      break;
    case LogMessageType::Error:
      prefix = L"ERROR: ";
      break;
    case LogMessageType::Debug:
      prefix = L"DEBUG: ";
      break;
  }
  std::wcout << prefix << message << std::endl;
}

} // namespace GameFramework::detail
