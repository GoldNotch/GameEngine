#pragma once
#include "Types.hpp"
#include <memory>
struct VulkanContext;
namespace vk
{
struct CommandBuffer;
class SurfaceKHR;
}

struct Renderer2D : public IRenderer
{
  Renderer2D(const VulkanContext & ctx, const vk::SurfaceKHR & surface);
  virtual ~Renderer2D();
  virtual void Render() const override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl = nullptr;
};
