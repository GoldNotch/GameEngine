#pragma once
#include <array>

#include <Plugin/Plugin.hpp>
#include <PluginInterfaces/WindowsPlugin.hpp>
#include <Render/Color.hpp>
#include <Render/Scene2d.hpp>
#include <Render/Scene3d.hpp>

namespace GameFramework
{

/// поверхность рисования
struct IDevice
{
  virtual ~IDevice() = default;
  virtual Scene2DUPtr AcquireScene2D() = 0;
  virtual Scene3DUPtr AcquireScene3D() = 0;
  virtual int GetOwnerId() const noexcept = 0;
  virtual float GetAspectRatio() const noexcept = 0;
};

/// поверхность рисования в окно
struct IScreenDevice : public IDevice
{
  virtual ~IScreenDevice() = default;
  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;
  virtual void Refresh() = 0;
  virtual void OnResize(int newWidth, int newHeight) = 0;
  virtual const IWindow & GetWindow() const & noexcept = 0;
};
using ScreenDeviceUPtr = std::unique_ptr<IScreenDevice>;

struct RenderPlugin : public IPluginInstance
{
  virtual ~RenderPlugin() = default;
  virtual ScreenDeviceUPtr CreateScreenDevice(IWindow & window) = 0;
  virtual void Tick() = 0;
};

} // namespace GameFramework
