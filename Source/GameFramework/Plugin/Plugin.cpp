#include "Plugin.hpp"

#include <dylib.hpp>

namespace GameFramework
{

struct DylibPluginLoader : public IPluginLoader
{
  explicit DylibPluginLoader(const std::filesystem::path & path);
  virtual ~DylibPluginLoader() override = default;

  virtual IPluginInstance * GetInstance() override { return m_instance.get(); }

private:
  dylib::library m_library;
  std::unique_ptr<IPluginInstance> m_instance;
};

DylibPluginLoader::DylibPluginLoader(const std::filesystem::path & path)
  : m_library(path, dylib::decorations::os_default())
{
  using CreateInstanceFunc = decltype(CreateInstance);
  auto createInstanceFunc = m_library.get_function<CreateInstanceFunc>("CreateInstance");
  if (createInstanceFunc)
    m_instance = createInstanceFunc();
}

GAME_FRAMEWORK_API std::unique_ptr<IPluginLoader> LoadPlugin(const std::filesystem::path & path)
{
  return std::make_unique<DylibPluginLoader>(path);
}

} // namespace GameFramework
