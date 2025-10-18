#pragma once
#include <functional>
#include <memory>
#include <string>


namespace Utils
{
using SurfaceDescriptor = std::pair<void *, void *>;

struct IWindow
{
  using ResizeCallback = std::function<void(int, int)>;


  virtual ~IWindow() = default;
  virtual SurfaceDescriptor GetSurface() const noexcept = 0;
  virtual std::pair<int, int> GetSize() const noexcept = 0;
  virtual float GetAspectRatio() const noexcept = 0;
  virtual void SetCursorHidden(bool hidden = true) = 0;
  virtual bool IsCursorHidden() const noexcept = 0;
  virtual void Close() = 0;
  virtual bool ShouldClose() const noexcept = 0;

  virtual void SetResizeCallback(ResizeCallback && callback) = 0;
};

std::unique_ptr<IWindow> NewWindow(const std::string & title, int width, int height);

} // namespace Utils


using WindowUPtr = std::unique_ptr<Utils::IWindow>;
