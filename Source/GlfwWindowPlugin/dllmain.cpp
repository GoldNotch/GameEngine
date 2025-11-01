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

private:
  GlfwInstance m_instance;
};

GameFramework::WindowUPtr GlfwPlugin::NewWindow(int id, const std::string & title, int width,
                                                int height)
{
  return GlfwWindowsPlugin::NewWindowImpl(id, title, width, height);
}

void GlfwPlugin::PollEvents()
{
  m_instance.PollEvents();
}

} // namespace GlfwWindowsPlugin

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance(
  const GameFramework::IPluginLoader & loader)
{
  return std::make_unique<GlfwWindowsPlugin::GlfwPlugin>();
}
