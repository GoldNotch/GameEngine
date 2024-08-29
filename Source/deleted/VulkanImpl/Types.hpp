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
struct VkDescriptorSetLayoutBinding;
struct VkDescriptorPoolSize;

struct VulkanContext;
struct RenderScene;
struct BufferGPU;

using VulkanHandler = void *;

struct IMemoryManager
{
  IMemoryManager() = default;
  virtual ~IMemoryManager() = default;
  virtual BufferGPU AllocBuffer(std::size_t size, uint32_t usage) const & = 0;
  virtual BufferGPU AllocMappedBuffer(std::size_t size, uint32_t usage) const & = 0;

protected:
  IMemoryManager(const IMemoryManager &) = delete;
  IMemoryManager & operator=(const IMemoryManager &) = delete;
};


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
struct ShaderAPIBuilder final
{
  // it's for vertex attributes
  static std::vector<VkVertexInputBindingDescription> BuildBindings() noexcept;
  static std::vector<VkVertexInputAttributeDescription> BuildAttributes() noexcept;
  // it's for resources (like uniforms, samplers, images)
  static std::vector<VkDescriptorSetLayoutBinding> BuildDescriptorsLayout() noexcept;
  static std::vector<VkDescriptorPoolSize> BuildPoolAllocationInfo() noexcept;
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
void ProcessWithPipeline(const IPipeline & pipeline, size_t frame_index,
                         const vk::CommandBuffer & buffer, const ObjT & obj);


/// @brief Renderer renders an image. It has own framebuffer, graphic queue, render pass and some pipelines
struct ISwapchain
{
  /*
    Renderer is a render-pass owner (framebuffer), also it declares a scene, owns it and renders it
    it owns some pipelines
  */

  ISwapchain() = default;
  virtual ~ISwapchain() = default;

  /// @brief rendering function
  /// @param scene
  virtual void Render(const RenderScene & scene) = 0;
  /// @brief get images count in swapchain queue
  /// @return
  virtual uint32_t GetImagesCountInFlight() const = 0;
  /// @brief invalidate rendering surface
  virtual void Invalidate() = 0;

protected:
  ISwapchain(const ISwapchain &) = delete;
  ISwapchain & operator=(const ISwapchain &) = delete;
};
