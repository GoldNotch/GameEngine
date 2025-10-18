#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>

namespace GameFramework
{
struct GAME_FRAMEWORK_API IPluginInstance
{
  virtual ~IPluginInstance() = default;
  //virtual std::string GetName() const = 0;
  //virtual int GetVersion() const = 0;
};

GAME_API std::unique_ptr<IPluginInstance> CreateInstance();

struct IPluginLoader
{
  virtual ~IPluginLoader() = default;
  virtual IPluginInstance * GetInstance() = 0;
};

GAME_FRAMEWORK_API std::unique_ptr<IPluginLoader> LoadPlugin(const std::filesystem::path & path);

} // namespace GameFramework
