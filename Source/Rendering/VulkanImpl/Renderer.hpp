#pragma once
#include <memory>

#include "Types.hpp"

namespace vk
{
class CommandBuffer;
class SurfaceKHR;
} // namespace vk
struct VulkanContext;

struct Renderer : public IRenderer
{
  Renderer(const VulkanContext & ctx, const vk::SurfaceKHR & surface);
  virtual ~Renderer();
  virtual void Render(const RenderScene & scene) override;
  /// @brief invalidate renderer's resources
  virtual void Invalidate() override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl = nullptr;
};
