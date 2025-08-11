#include "GameFramework.hpp"

#include <iostream>

#include "Resources/ResourceLoader.hpp"

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

template<>
std::unique_ptr<IStaticMeshResouce> ConstructResource<IStaticMeshResouce>(
  const std::filesystem::path & path)
{
  return CreateStaticMeshResource(path);
}

GAME_FRAMEWORK_API IStaticMeshResouce * LoadStaticMesh(const char * path)
{
  return g_resourceLoader.GetResource<IStaticMeshResouce>(path);
}

} // namespace GameFramework
