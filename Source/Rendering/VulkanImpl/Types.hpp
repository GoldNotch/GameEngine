#pragma once

namespace vk
{
class CommandBuffer;
struct SubpassDescription;
} // namespace vk


template<typename PipelineT>
struct SubpassDescriptionBuilder final
{
  static vk::SubpassDescription Get() noexcept;
};

struct IPipeline
{
  /*
    Pipeline owns vulkan pipeline and makes subpass description to intergrate with renderer  
  */
  virtual ~IPipeline() = default;
  virtual void Process(const vk::CommandBuffer & buffer) const = 0;
};

/// @brief Renderer renders an image. It has own framebuffer, graphic queue, render pass and some pipelines
struct IRenderer
{
  /*
    Renderer is a render-pass owner (framebuffer), also it declares a scene, owns it and renders it
    it owns some pipelines
  */

  virtual ~IRenderer() = default;

  virtual void Render() const = 0;
};
