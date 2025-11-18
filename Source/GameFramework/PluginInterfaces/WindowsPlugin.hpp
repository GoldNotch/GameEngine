#pragma once

#include <functional>
#include <memory>
#include <string>

#include <Input/InputController.hpp>
#include <Plugin/Plugin.hpp>

namespace GameFramework
{
using SurfaceDescriptor = std::pair<void *, void *>;

struct IWindow
{
  using ResizeCallback = std::function<void(int, int)>;

  virtual ~IWindow() = default;
  virtual int GetId() const noexcept = 0;
  virtual std::string GetTitle() const noexcept = 0;
  virtual SurfaceDescriptor GetSurface() const noexcept = 0;
  virtual std::pair<int, int> GetSize() const noexcept = 0;
  virtual float GetAspectRatio() const noexcept = 0;
  virtual void SetCursorHidden(bool hidden = true) = 0;
  virtual bool IsCursorHidden() const noexcept = 0;
  virtual void Close() = 0;
  virtual bool ShouldClose() const noexcept = 0;
  virtual InputBackend & GetInput() & noexcept = 0;

  virtual void SetResizeCallback(ResizeCallback && callback) = 0;
};

using WindowUPtr = std::unique_ptr<IWindow>;

struct WindowsPlugin : public IPluginInstance
{
  virtual ~WindowsPlugin() = default;
  virtual WindowUPtr NewWindow(int id, const std::string & title, int width, int height) = 0;
  virtual void PollEvents() = 0;
};


} // namespace GameFramework
