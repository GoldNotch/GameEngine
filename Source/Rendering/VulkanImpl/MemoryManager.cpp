#include "MemoryManager.hpp"

#include "VulkanContext.hpp"
#define VMA_IMPLEMENTATION
#include <Formatter.hpp>

#include "vk_mem_alloc.h"


struct MemoryManager::Impl
{
  VmaAllocation t;
  explicit Impl(const VulkanContext & ctx);
  ~Impl();

  BufferGPU AllocBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;

private:
  VmaAllocator allocator;
};

MemoryManager::MemoryManager(const VulkanContext & ctx)
  : impl(new Impl(ctx))
{
}

MemoryManager::~MemoryManager() noexcept
{
  impl.reset();
}

BufferGPU MemoryManager::AllocBuffer(std::size_t size, uint32_t usage) const &
{
  return impl->AllocBuffer(size, static_cast<VkBufferUsageFlags>(usage));
}

// --------------------- Implementation ----------------------

MemoryManager::Impl::Impl(const VulkanContext & ctx)
{
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.instance = ctx.GetInstance();
  allocator_info.physicalDevice = ctx.GetGPU();
  allocator_info.device = ctx.GetDevice();
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;
  if (auto res = vmaCreateAllocator(&allocator_info, &allocator); res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create buffer_allocator - " << res);
}

MemoryManager::Impl::~Impl()
{
  vmaDestroyAllocator(allocator);
}

BufferGPU MemoryManager::Impl::AllocBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const
{
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  VkBuffer buffer;
  VmaAllocation allocation;
  vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
  return BufferGPU(buffer, allocation, size, allocator);
}


BufferGPU::BufferGPU(vk::Buffer buffer, void * allocation, std::size_t size,
                     void * allocator) noexcept
  : handler(static_cast<VkBuffer>(buffer))
  , mem_block(allocation)
  , allocator(allocator)
  , mem_size(size)
{
}

BufferGPU::~BufferGPU() noexcept
{
  if (allocator != nullptr && handler != VK_NULL_HANDLE)
    vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(allocator), reinterpret_cast<VkBuffer>(handler),
                     reinterpret_cast<VmaAllocation>(mem_block));
}

BufferGPU::BufferGPU(BufferGPU && other) noexcept
{
  std::swap(allocator, other.allocator);
  std::swap(handler, other.handler);
  std::swap(mem_block, other.mem_block);
  std::swap(mem_size, other.mem_size);
}

BufferGPU & BufferGPU::operator=(BufferGPU && other) noexcept
{
  std::swap(allocator, other.allocator);
  std::swap(handler, other.handler);
  std::swap(mem_block, other.mem_block);
  std::swap(mem_size, other.mem_size);
  return *this;
}

BufferGPU::operator vk::Buffer() const noexcept
{
  return reinterpret_cast<VkBuffer>(handler);
}


BufferGPU::ScopeMapper BufferGPU::Map() const
{
  void * mapped_memory = nullptr;
  if (VkResult res = vmaMapMemory(reinterpret_cast<VmaAllocator>(allocator),
                                  reinterpret_cast<VmaAllocation>(mem_block), &mapped_memory);
      res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to map buffer memory - " << res);

  return BufferGPU::ScopeMapper(mapped_memory,
                                [this](void * mem_ptr)
                                {
                                  if (mem_ptr != nullptr)
                                    vmaUnmapMemory(reinterpret_cast<VmaAllocator>(allocator),
                                                   reinterpret_cast<VmaAllocation>(mem_block));
                                });
}

void BufferGPU::Flush() const noexcept
{
  vmaFlushAllocation(reinterpret_cast<VmaAllocator>(allocator),
                     reinterpret_cast<VmaAllocation>(mem_block), 0, mem_size);
}
