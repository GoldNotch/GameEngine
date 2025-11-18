#include <GameFramework.hpp>

#include "GlfwInstance.hpp"
#include "GlfwWindow.hpp"

namespace GlfwWindowsPlugin
{

struct GlfwPlugin : public GameFramework::WindowsPlugin
{
  ~GlfwPlugin() override = default;
  virtual GameFramework::WindowUPtr NewWindow(int id, const std::string & title, int width,
                                              int height) override;
  virtual void PollEvents() override;
};

GameFramework::WindowUPtr GlfwPlugin::NewWindow(int id, const std::string & title, int width,
                                                int height)
{
  return std::make_unique<GlfwWindow>(id, title, width, height);
}

void GlfwPlugin::PollEvents()
{
  GetGlfwInstance().PollEvents();
}

} // namespace GlfwWindowsPlugin

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance(
  const GameFramework::IPluginLoader & loader)
{
  return std::make_unique<GlfwWindowsPlugin::GlfwPlugin>();
}
