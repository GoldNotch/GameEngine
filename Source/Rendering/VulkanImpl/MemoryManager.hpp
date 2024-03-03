#pragma once
#include <functional>
#include <memory>

#include "Types.hpp"
struct VulkanContext;
namespace vk
{
class Buffer;
}


/// @brief Special structure to store the buffer handler and allocated memory block
struct BufferGPU final
{
  using UnmapFunc = std::function<void(void *)>;
  using ScopeMapper = std::unique_ptr<void, UnmapFunc>;

  BufferGPU() = default;
  ~BufferGPU() noexcept;
  BufferGPU(BufferGPU && other) noexcept;
  BufferGPU & operator=(BufferGPU && other) noexcept;

  operator vk::Buffer() const noexcept;

  /// @brief returns size in bytes
  inline std::size_t size() const noexcept { return mem_size; }

  /// @brief map
  /// @return
  ScopeMapper Map() const;
  void Flush() const noexcept;

private:
  friend struct MemoryManager;
  VulkanHandler handler = nullptr;
  VulkanHandler mem_block = nullptr;
  VulkanHandler allocator = nullptr;
  std::size_t mem_size = 0;

private:
  BufferGPU(vk::Buffer buffer, void * allocation, std::size_t size, void * allocator) noexcept;
  BufferGPU(const BufferGPU &) = delete;
  BufferGPU & operator=(const BufferGPU &) = delete;
};

std::unique_ptr<IMemoryManager> CreateMemoryManager(const VulkanContext & ctx);
