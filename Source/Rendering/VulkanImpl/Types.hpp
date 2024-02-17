#pragma once
#include <vector>
namespace vk
{
class CommandBuffer;
struct SubpassDescription;
} // namespace vk

struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

template<typename PipelineT>
struct SubpassDescriptionBuilder final
{
  static vk::SubpassDescription Get() noexcept;
};

template<typename VertexDataT>
struct VertexStateDescriptionBuilder final
{
  static std::vector<VkVertexInputBindingDescription> BuildBindings() noexcept;
  static std::vector<VkVertexInputAttributeDescription> BuildAttributes() noexcept;
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

  virtual void Render() = 0;
  virtual void Invalidate() = 0;
};
