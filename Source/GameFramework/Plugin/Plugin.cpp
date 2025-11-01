#include "Plugin.hpp"

#include <dylib.hpp>

namespace GameFramework
{

struct DylibPluginLoader : public IPluginLoader
{
  explicit DylibPluginLoader(const std::filesystem::path & path);
  virtual ~DylibPluginLoader() override = default;

  virtual IPluginInstance * GetInstance() override { return m_instance.get(); }
  virtual const std::filesystem::path & Path() const & noexcept override;

private:
  std::filesystem::path m_path;
  dylib::library m_library;
  std::unique_ptr<IPluginInstance> m_instance;
};

DylibPluginLoader::DylibPluginLoader(const std::filesystem::path & path)
  : m_path(path)
  , m_library(path, dylib::decorations::os_default())
{
  using CreateInstanceFunc = decltype(CreateInstance);
  auto createInstanceFunc = m_library.get_function<CreateInstanceFunc>("CreateInstance(IPluginLoader const &)");
  if (createInstanceFunc)
    m_instance = createInstanceFunc(*this);
}

const std::filesystem::path & DylibPluginLoader::Path() const & noexcept
{
  return m_path;
}

GAME_FRAMEWORK_API std::unique_ptr<IPluginLoader> LoadPlugin(const std::filesystem::path & path)
{
  return std::make_unique<DylibPluginLoader>(path);
}

} // namespace GameFramework
