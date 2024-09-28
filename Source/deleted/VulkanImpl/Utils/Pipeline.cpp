#include "Pipeline.hpp"
#include "DescriptorsBuffer.hpp"


namespace
{
vk::DescriptorPool CreateDescriptorsPool(const VulkanContext & ctx, const ISwapchain & renderer)
{
  auto poolSizes = ShaderAPIBuilder<StaticMesh>::BuildPoolAllocationInfo();
  uint32_t images_in_flight = renderer.GetImagesCountInFlight();
  for (auto && size : poolSizes)
    size.descriptorCount *= images_in_flight;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = std::accumulate(poolSizes.begin(), poolSizes.end(), 0u,
                                     [](uint32_t acc, const VkDescriptorPoolSize & pool_size)
                                     { return acc + pool_size.descriptorCount; });

  VkDescriptorPool c_pool;
  if (vkCreateDescriptorPool(ctx.GetDevice(), &poolInfo, nullptr, &c_pool) != VK_SUCCESS)
    throw std::runtime_error("failed to create descriptor pool!");
  return vk::DescriptorPool(c_pool);
}

std::vector<VkDescriptorSet> CreateDescriptorSet(const VulkanContext & ctx,
                                                 const ISwapchain & renderer, VkDescriptorPool pool,
                                                 VkDescriptorSetLayout layout)
{
  uint32_t images_in_flight = renderer.GetImagesCountInFlight();

  std::vector<VkDescriptorSetLayout> layouts(images_in_flight, layout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(images_in_flight);
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> descriptor_sets(images_in_flight, VK_NULL_HANDLE);
  if (vkAllocateDescriptorSets(ctx.GetDevice(), &allocInfo, descriptor_sets.data()) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate descriptor sets!");
  return descriptor_sets;
}


void LinkBufferToDescriptor(const VulkanContext & ctx, VkDescriptorSet set, VkDescriptorType type,
                            const VkDescriptorBufferInfo & bufferInfo)
{
  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = set;
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = type;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &bufferInfo;
  descriptorWrite.pImageInfo = nullptr;       // Optional
  descriptorWrite.pTexelBufferView = nullptr; // Optional

  vkUpdateDescriptorSets(ctx.GetDevice(), 1, &descriptorWrite, 0, nullptr);
}

VkBufferUsageFlags DescriptorType2BufferType(VkDescriptorType type)
{
  switch (type)
  {
    /*case VK_DESCRIPTOR_TYPE_SAMPLER:
      return VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;*/
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    default:
      return 0;
  }
}
} // namespace

namespace vk::utils
{
Pipeline::Pipeline(const VulkanContext & ctx, const ISwapchain & renderer, VkPipeline pipeline,
                   VkPipelineLayout layout, VkDescriptorSetLayout descriptors_layout) noexcept
  : ctx(ctx)
  , renderer(renderer)
  , pipeline(pipeline)
  , layout(layout)
  , descriptors_layout(descriptors_layout)
{
  pool = CreateDescriptorsPool(ctx, renderer);
}

void Pipeline::CreateUniformDescriptors(VkDescriptorType type, size_t count)
{
  for (; count > 0; --count)
    descriptor_bindings.emplace_back(ctx, renderer, pool, descriptors_layout, type);
}

Pipeline::~Pipeline() noexcept
{
  vkDestroyPipelineLayout(ctx.GetDevice(), layout, nullptr);
  vkDestroyDescriptorPool(ctx.GetDevice(), pool, nullptr);
  vkDestroyDescriptorSetLayout(ctx.GetDevice(), descriptors_layout, nullptr);
  vkDestroyPipeline(ctx.GetDevice(), pipeline, nullptr);
}
} // namespace vk::utils


namespace vk::utils
{
DescriptorBinding::DescriptorBinding(const VulkanContext & ctx, const ISwapchain & renderer,
                                     vk::DescriptorPool & pool,
                                     vk::DescriptorSetLayout & descr_layout, VkDescriptorType type)
  : descriptors_layout(descr_layout)
  , ctx(ctx)
  , type(type)
{
  descriptor_sets = CreateDescriptorSet(ctx, renderer, pool, descriptors_layout);
}

void DescriptorBinding::Alloc(size_t size, bool permanently_mapped /*= false*/)
{
  size_t n = descriptor_sets.size();
  buffers.resize(n);
  if (permanently_mapped)
    mapped_pointers.resize(n);
  for (size_t i = 0; i < n; ++i)
  {
    buffers[i] =
      std::move(ctx.GetMemoryManager().AllocMappedBuffer(size, ::DescriptorType2BufferType(type)));
    if (permanently_mapped)
      mapped_pointers[i] = buffers[i].Map();

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = static_cast<vk::Buffer>(buffers[i]);
    bufferInfo.offset = 0;
    bufferInfo.range = size;
    ::LinkBufferToDescriptor(ctx, descriptor_sets[i], type, bufferInfo);
  }
}


void DescriptorBinding::Upload(const void * data, size_t size, size_t offset /* = 0*/)
{
  if (mapped_pointers.empty())
  {
    for (auto && buffer : buffers)
    {
      auto mapped_pointer = buffer.Map();
      std::memcpy(mapped_pointer.get(), reinterpret_cast<const char *>(data) + offset, size);
      buffer.Flush();
    }
  }
  else
  {
    for (auto && ptr : mapped_pointers)
      std::memcpy(ptr.get(), reinterpret_cast<const char *>(data) + offset, size);
  }
}

void DescriptorBinding::Bind(const vk::CommandBuffer & buffer, const vk::PipelineLayout & layout,
                             uint32_t frame_index)
{
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1,
                          &descriptor_sets[frame_index], 0, nullptr);
}


} // namespace vk::utils
