#include <Constants.hpp>
#include <GameFramework.hpp>
#include <ScreenDevice.hpp>

namespace RenderPlugin
{
struct RenderPlugin_RHI : public GameFramework::RenderPlugin
{
  explicit RenderPlugin_RHI(const GameFramework::IPluginLoader & loader);
  virtual ~RenderPlugin_RHI() = default;

  virtual GameFramework::ScreenDeviceUPtr CreateScreenDevice(
    GameFramework::IWindow & window) override;

  virtual void Tick() override;

private:
  std::unique_ptr<RHI::IContext> m_context;
};

RenderPlugin_RHI::RenderPlugin_RHI(const GameFramework::IPluginLoader & loader)
{
  GameFramework::GetFileManager().Mount(g_shadersDirectory,
                                        GameFramework::CreateDirectoryMountPoint(
                                          loader.Path() / g_shadersDirectory));
  RHI::GpuTraits gpuTraits{};
  gpuTraits.require_presentation = true;
  m_context = CreateContext(gpuTraits, nullptr);
}

GameFramework::ScreenDeviceUPtr RenderPlugin_RHI::CreateScreenDevice(
  GameFramework::IWindow & window)
{
  return std::make_unique<ScreenDevice>(*m_context, window);
}

void RenderPlugin_RHI::Tick()
{
  m_context->ClearResources();
  m_context->TransferPass();
}

} // namespace RenderPlugin

/// creates global game instance
PLUGIN_API std::unique_ptr<GameFramework::IPluginInstance> CreateInstance(
  const GameFramework::IPluginLoader & loader)
{
  return std::make_unique<RenderPlugin::RenderPlugin_RHI>(loader);
}
