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

struct IPluginLoader
{
  virtual ~IPluginLoader() = default;
  virtual IPluginInstance * GetInstance() = 0;
  virtual const std::filesystem::path & Path() const & noexcept = 0;
};

PLUGIN_API std::unique_ptr<IPluginInstance> CreateInstance(const IPluginLoader & loader);
GAME_FRAMEWORK_API std::unique_ptr<IPluginLoader> LoadPlugin(const std::filesystem::path & path);

} // namespace GameFramework
