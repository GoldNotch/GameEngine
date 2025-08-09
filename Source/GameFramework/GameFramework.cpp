#include "GameFramework.hpp"

#include <iostream>

#include "Resources/ResourceLoader.hpp"
#include "Resources/StaticMesh.hpp"
namespace GameFramework
{

namespace detail
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
} // namespace detail

static ResourceLoader g_resourceLoader;

GAME_FRAMEWORK_API IResource * LoadStaticMesh(const char * path)
{
  return g_resourceLoader.GetResource<StaticMeshResource>(path);
}

} // namespace GameFramework
