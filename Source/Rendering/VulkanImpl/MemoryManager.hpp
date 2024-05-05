#pragma once
#include <array>
#include <functional>
#include <memory>

#include "Types.hpp"
struct VulkanContext;
namespace vk
{
class Buffer;
}


/// @brief Special structure to store the buffer handler and allocated memory block
struct BufferGPU
{
  using UnmapFunc = std::function<void(void *)>;
  using ScopedPointer = std::unique_ptr<void, UnmapFunc>;

  BufferGPU() = default;
  ~BufferGPU() noexcept;
  BufferGPU(BufferGPU && other) noexcept;
  BufferGPU & operator=(BufferGPU && other) noexcept;

  operator vk::Buffer() const noexcept;

  /// @brief returns size in bytes
  inline std::size_t size() const noexcept { return mem_size; }

  /// @brief map
  /// @return
  ScopedPointer Map() const;
  void Flush() const noexcept;

private:
  friend struct MemoryManager;
  using AllocInfoRawMemory = std::array<uint32_t, 14>;
  AllocInfoRawMemory alloc_info;
  VulkanHandler handler = nullptr;
  VulkanHandler mem_block = nullptr;
  VulkanHandler allocator = nullptr;
  std::size_t mem_size = 0;
  uint32_t flags = 0;

private:
  bool IsMapped() const;

private:
  BufferGPU(vk::Buffer buffer, void * allocation, std::size_t size, void * allocator,
            AllocInfoRawMemory alloc_info, uint32_t flags) noexcept;
  BufferGPU(const BufferGPU &) = delete;
  BufferGPU & operator=(const BufferGPU &) = delete;
};

std::unique_ptr<IMemoryManager> CreateMemoryManager(const VulkanContext & ctx);
