#pragma once
#include <array>

#include <Plugin/Plugin.hpp>
#include <PluginInterfaces/WindowsPlugin.hpp>
#include <Render/Rect2d.hpp>

namespace GameFramework
{
using Color3f = std::array<float, 3>; //rgb
using Color4f = std::array<float, 4>; //rgba

struct IRenderableScene2D
{
  //destructor will draw objects
  virtual ~IRenderableScene2D() = default;
  virtual void SetBackground(const Color3f & color) = 0;
  //virtual void DrawCube(/*transform*/) = 0;
  //virtual void DrawSphere(/*transform*/) = 0;
  virtual void AddRect(const Rect2d & rect) = 0;
};
using Scene2DUPtr = std::unique_ptr<IRenderableScene2D>;

/// поверхность рисования
struct IDevice
{
  virtual ~IDevice() = default;
  virtual Scene2DUPtr AcquireScene2D() = 0;
  virtual int GetOwnerId() const noexcept = 0;
};

/// поверхность рисования в окно
struct IScreenDevice : public IDevice
{
  virtual ~IScreenDevice() = default;
  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;
  virtual void Refresh() = 0;
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
