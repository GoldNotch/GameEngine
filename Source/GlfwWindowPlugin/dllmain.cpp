#include <PluginInterfaces/WindowPlugin.hpp>

#include "GlfwInstance.hpp"
#include "GlfwWindow.hpp"

namespace GlfwPlugin
{

struct GlfwWindowsPlugin : public GameFramework::WindowsPlugin
{
  ~GlfwWindowsPlugin() override = default;
  virtual GameFramework::WindowUPtr NewWindow(const std::string & title, int width,
                                              int height) override;
  virtual void PollEvents() override;

private:
  GlfwInstance m_instance;
};

GameFramework::WindowUPtr GlfwWindowsPlugin::NewWindow(const std::string & title, int width,
                                                       int height)
{
  return GlfwPlugin::NewWindowImpl(title, width, height);
}

void GlfwWindowsPlugin::PollEvents()
{
  m_instance.PollEvents();
}

} // namespace GlfwPlugin

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance()
{
  return std::make_unique<GlfwPlugin::GlfwWindowsPlugin>();
}
