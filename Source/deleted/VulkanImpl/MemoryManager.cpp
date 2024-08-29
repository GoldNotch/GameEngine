#include "MemoryManager.hpp"

#include "VulkanContext.hpp"
#define VMA_IMPLEMENTATION
#include <Formatter.hpp>

#include "vk_mem_alloc.h"

namespace
{
std::tuple<VkBuffer, VmaAllocation, VmaAllocationInfo> CreateVMABuffer(
  VmaAllocator allocator, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flags,
  VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO)
{
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;

  VmaAllocationCreateInfo allocCreateInfo = {};
  allocCreateInfo.usage = memory_usage;
  allocCreateInfo.flags = flags;
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocInfo;
  sizeof(VmaAllocatorInfo);
  vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation, &allocInfo);
  return {buffer, allocation, allocInfo};
}
} // namespace

struct MemoryManager final : public IMemoryManager
{
  friend struct BufferGPU;
  explicit MemoryManager(const VulkanContext & ctx);
  virtual ~MemoryManager() override;

  virtual BufferGPU AllocBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const & override;
  virtual BufferGPU AllocMappedBuffer(std::size_t size, uint32_t usage) const & override;

private:
  VmaAllocator allocator;
};

// --------------------- Implementation ----------------------

MemoryManager::MemoryManager(const VulkanContext & ctx)
{
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.instance = ctx.GetInstance();
  allocator_info.physicalDevice = ctx.GetGPU();
  allocator_info.device = ctx.GetDevice();
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;
  if (auto res = vmaCreateAllocator(&allocator_info, &allocator); res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create buffer_allocator - " << res);
}

MemoryManager::~MemoryManager()
{
  vmaDestroyAllocator(allocator);
}

BufferGPU MemoryManager::AllocBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const &
{
  VmaAllocationCreateFlags allocation_flags =
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  auto [buffer, allocation, alloc_info] =
    ::CreateVMABuffer(allocator, size, usage, allocation_flags);
  return BufferGPU(buffer, allocation, size, allocator,
                   reinterpret_cast<BufferGPU::AllocInfoRawMemory &>(alloc_info), allocation_flags);
}


BufferGPU MemoryManager::AllocMappedBuffer(std::size_t size, uint32_t usage) const &
{
  VmaAllocationCreateFlags allocation_flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  auto [buffer, allocation, alloc_info] =
    ::CreateVMABuffer(allocator, size, usage, allocation_flags);
  return BufferGPU(buffer, allocation, size, allocator,
                   reinterpret_cast<BufferGPU::AllocInfoRawMemory &>(alloc_info), allocation_flags);
}


BufferGPU::BufferGPU(vk::Buffer buffer, void * allocation, std::size_t size, void * allocator,
                     AllocInfoRawMemory alloc_info, uint32_t flags) noexcept
  : handler(static_cast<VkBuffer>(buffer))
  , mem_block(allocation)
  , allocator(allocator)
  , mem_size(size)
  , flags(flags)
  , alloc_info(alloc_info)
{
  static_assert(
    sizeof(VmaAllocationInfo) <= sizeof(BufferGPU::AllocInfoRawMemory),
    "size of BufferGPU::AllocInfoRawMemory less then should be. It should be at least as VmaAllocationInfo");
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


BufferGPU::ScopedPointer BufferGPU::Map() const
{
  void * mapped_memory = nullptr;
  const bool is_mapped = IsMapped();
  if (!is_mapped)
  {
    if (VkResult res = vmaMapMemory(reinterpret_cast<VmaAllocator>(allocator),
                                    reinterpret_cast<VmaAllocation>(mem_block), &mapped_memory);
        res != VK_SUCCESS)
      throw std::runtime_error(Formatter() << "Failed to map buffer memory - " << res);
  }
  else
  {
    mapped_memory = reinterpret_cast<const VmaAllocationInfo &>(alloc_info).pMappedData;
  }

  return BufferGPU::ScopedPointer(mapped_memory,
                                  [this, is_mapped](void * mem_ptr)
                                  {
                                    if (!is_mapped && mem_ptr != nullptr)
                                      vmaUnmapMemory(reinterpret_cast<VmaAllocator>(allocator),
                                                     reinterpret_cast<VmaAllocation>(mem_block));
                                  });
}

void BufferGPU::Flush() const noexcept
{
  vmaFlushAllocation(reinterpret_cast<VmaAllocator>(allocator),
                     reinterpret_cast<VmaAllocation>(mem_block), 0, mem_size);
}


bool BufferGPU::IsMapped() const
{
  return (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;
}


std::unique_ptr<IMemoryManager> CreateMemoryManager(const VulkanContext & ctx)
{
  return std::make_unique<MemoryManager>(ctx);
}
