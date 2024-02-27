#pragma once
#include <functional>
#include <memory>

struct VulkanContext;
namespace vk
{
class Buffer;
}


/*
Вопросы на подумать:
1) кто хранит map загруженных буферов?
2) map по буферам или по объектам (один объект может иметь вершинный буфер + индексный буфер)
3) Что использовать как ключ для буфера?
*/
struct MemoryManager final
{
  struct BufferGPU;

  explicit MemoryManager(const VulkanContext & ctx);
  ~MemoryManager() noexcept;

  /// @brief allocate buffer in GPU
  /// @param size - size of buffer in bytes
  /// @param usage - VkBufferUsageFlags - flags to define what is the buffer
  BufferGPU AllocBuffer(std::size_t size, uint32_t usage) const &;

private:
  struct Impl;
  std::unique_ptr<Impl> impl = nullptr;

private:
  MemoryManager(const MemoryManager &) = delete;
  MemoryManager & operator=(const MemoryManager &) = delete;
};


/// @brief Special structure to store the buffer handler and allocated memory block
struct MemoryManager::BufferGPU final
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
  friend struct MemoryManager::Impl;
  void * handler = nullptr;
  void * mem_block = nullptr;
  void * allocator = nullptr;
  std::size_t mem_size = 0;

private:
  BufferGPU(vk::Buffer buffer, void * allocation, std::size_t size, void * allocator) noexcept;
  BufferGPU(const BufferGPU &) = delete;
  BufferGPU & operator=(const BufferGPU &) = delete;
};
