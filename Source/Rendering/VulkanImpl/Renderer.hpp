#pragma once
#include <memory>

#include "Types.hpp"
struct VulkanContext;
namespace vk
{
class CommandBuffer;
class SurfaceKHR;
} // namespace vk

struct Renderer : public IRenderer
{
  Renderer(const VulkanContext & ctx, const vk::SurfaceKHR & surface);
  virtual ~Renderer();
  virtual void Render() override;
  /// @brief invalidate renderer's resources
  virtual void Invalidate() override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl = nullptr;
};
