#pragma once
#include <cassert>
#include <vector>
namespace vk
{
class CommandBuffer;
struct SubpassDescription;
struct Rect2D;
class RenderPass;
class Pipeline;
class PipelineLayout;
} // namespace vk
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

struct VulkanContext;
struct RenderScene;

using VulkanHandler = void *;

/// @brief creates vk::SubpassDescription for special type of VertexData (actually for corresponding pipeline)
/// @tparam VertexDataT - type of drawable data
template<typename VertexDataT>
struct SubpassDescriptionBuilder final
{
  static vk::SubpassDescription Get() noexcept;
};

/// @brief creates some vulkan resources to link shader attributes declaration with vertex data format
/// @tparam VertexDataT - type of drawable data
template<typename VertexDataT>
struct VertexStateDescriptionBuilder final
{
  static std::vector<VkVertexInputBindingDescription> BuildBindings() noexcept;
  static std::vector<VkVertexInputAttributeDescription> BuildAttributes() noexcept;
};

/// @brief pipeline interface. Pipeline processes one type of objects and can draw it
struct IPipeline
{
  IPipeline() = default;
  virtual ~IPipeline() noexcept = default;
  /// @brief some logic in the beginning of processing (f.e. some preparations to rendering)
  /// @param buffer - command buffer
  /// @param viewport - settings for pipeline
  virtual void BeginProcessing(const vk::CommandBuffer & buffer,
                               const vk::Rect2D & viewport) const = 0;
  /// @brief Some logic in the end of processing (f.e. flush all commands in buffer)
  /// @param buffer - command buffer
  virtual void EndProcessing(const vk::CommandBuffer & buffer) const = 0;

protected:
  IPipeline(const IPipeline &) = delete;
  IPipeline & operator=(const IPipeline &) = delete;
};

/// @brief takes one drawable data object and process it with pipeline
/// @tparam ObjT - type of drawable data
/// @param pipeline - pipeline to process it
/// @param obj - drawable data
template<typename ObjT>
void ProcessWithPipeline(const IPipeline & pipeline, const vk::CommandBuffer & buffer,
                         const ObjT & obj);


/// @brief Renderer renders an image. It has own framebuffer, graphic queue, render pass and some pipelines
struct IRenderer
{
  /*
    Renderer is a render-pass owner (framebuffer), also it declares a scene, owns it and renders it
    it owns some pipelines
  */

  virtual ~IRenderer() = default;

  virtual void Render(const RenderScene & scene) = 0;
  virtual void Invalidate() = 0;
};
