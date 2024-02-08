#pragma once
#include "Types.hpp"
#include <memory>
struct VulkanContext;
namespace vk
{
class CommandBuffer;
class SurfaceKHR;
}

struct Renderer : public IRenderer
{
  Renderer(const VulkanContext & ctx, const vk::SurfaceKHR & surface);
  virtual ~Renderer();
  virtual void Render() const override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl = nullptr;
};
