#pragma once
#include "../VulkanContext.hpp"

namespace vk::utils
{
struct DescriptorBinding
{
  DescriptorBinding(const VulkanContext & ctx, const IRenderer & renderer,
                    vk::DescriptorPool & pool, vk::DescriptorSetLayout & descr_layout,
                    VkDescriptorType type);

  void Alloc(size_t size, bool permanently_mapped = false);
  void Upload(const void * data, size_t size, size_t offset = 0);
  void Bind(const vk::CommandBuffer & buffer, const vk::PipelineLayout& layout, uint32_t frame_index);


private:
  const VulkanContext & ctx;
  vk::DescriptorSetLayout & descriptors_layout;
  VkDescriptorType type;
  std::vector<VkDescriptorSet> descriptor_sets;          ///< one for each frame
  std::vector<BufferGPU> buffers;                        ///< one buffer for each frame
  std::vector<BufferGPU::ScopedPointer> mapped_pointers; ///< one for each buffer
};


struct Pipeline final
{
  friend struct PipelineBuilder;

  ~Pipeline() noexcept;
  VkPipeline GetPipeline() const noexcept { return pipeline; }
  VkPipelineLayout GetPipelineLayout() const noexcept { return layout; }
  DescriptorBinding & GetUniformBinding(size_t index) & { return descriptor_bindings[index]; }
  const DescriptorBinding & GetUniformBinding(size_t index) const &
  {
    return descriptor_bindings[index];
  }

private:
  const VulkanContext & ctx;
  const IRenderer & renderer;
  vk::Pipeline pipeline;
  vk::PipelineLayout layout;
  vk::DescriptorPool pool;
  vk::DescriptorSetLayout descriptors_layout;
  std::vector<DescriptorBinding> descriptor_bindings;


private:
  Pipeline(const VulkanContext & ctx, const IRenderer & renderer, VkPipeline pipeline,
           VkPipelineLayout layout, VkDescriptorSetLayout decriptors_layout) noexcept;

  void CreateUniformDescriptors(VkDescriptorType type, size_t count);

private:
  Pipeline(const Pipeline &) = delete;
  Pipeline & operator=(const Pipeline &) = delete;
};
} // namespace vk::utils
