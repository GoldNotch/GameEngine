#pragma once
#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
//#else //TODO UNIX
#endif
#endif // USE_WINDOW_OUTPUT

#include <filesystem>

#include <StaticString.hpp>
#include <Storage.hpp>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include "../RenderingSystem.h"
#include "MemoryManager.hpp"
#include "Types.hpp"


/// @brief context is object contains vulkan logical device. Also it provides access to vulkan functions
///			If rendering system uses several GPUs, you should create one context for each physical device
struct VulkanContext final
{
  /// @brief constructor
  explicit VulkanContext(const usRenderingOptions & opts);
  /// @brief destructor
  ~VulkanContext() noexcept;

  /// @brief returns dispatch_table, so user could write ctx->{any vulkan function}
  const vkb::DispatchTable * operator->() const { return &dispatch_table; }

  /// @brief Get renderer
  IRenderer * GetRenderer() const { return renderer.get(); }
  const vkb::Instance & GetInstance() const & { return vulkan_instance; }
  const vkb::Device & GetDevice() const & { return device; }
  const vkb::PhysicalDevice & GetGPU() const & { return choosen_gpu; }
  const IMemoryManager & GetMemoryManager() const & { return *memory_manager; }

  // TODO: Make it global functions
  /// @brief returns queue of specific type. doesn't own it
  std::pair<uint32_t, vk::Queue> GetQueue(vkb::QueueType type) const;
  /// @brief creates semaphore, doesn't own it
  vk::Semaphore CreateVkSemaphore() const;
  /// @brief creates fence, doesn't own it
  vk::Fence CreateFence(bool locked = false) const;
  /// @brief creates command pool for thread, doesn't own it
  vk::CommandPool CreateCommandPool(uint32_t queue_family_index) const;
  /// @brief creates command buffer in pool, doesn't own it
  vk::CommandBuffer CreateCommandBuffer(vk::CommandPool pool) const;

private:
  vkb::Instance vulkan_instance;
  vkb::PhysicalDevice choosen_gpu;
  vkb::Device device;
  vkb::DispatchTable dispatch_table;

  std::unique_ptr<IRenderer> renderer;
  std::unique_ptr<IMemoryManager> memory_manager;

private:
  VulkanContext(const VulkanContext &) = delete;
  VulkanContext & operator=(const VulkanContext &) = delete;
};


namespace vk::utils
{
template<std::size_t Size>
constexpr decltype(auto) ResolveShaderPath(const char (&filename)[Size])
{
  return Core::static_string(DATA_PATH) + Core::static_string("/Shaders/Vulkan/") +
         Core::static_string(filename);
}
} // namespace vk::utils


struct RenderScene
{
  Core::HeterogeneousStorage<StaticMesh> objects;
};


template<>
struct std::hash<glVec2>
{
  std::size_t operator()(glVec2 const & vertex) const noexcept
  {
    std::size_t result = 0;
    Core::utils::hash_combine(result, vertex.x);
    Core::utils::hash_combine(result, vertex.y);
    return result;
  }
};

template<>
struct std::hash<glVec3>
{
  std::size_t operator()(glVec3 const & vertex) const noexcept
  {
    std::size_t result = 0;
    Core::utils::hash_combine(result, vertex.x);
    Core::utils::hash_combine(result, vertex.y);
    Core::utils::hash_combine(result, vertex.z);
    return result;
  }
};

template<>
struct std::hash<glVec4>
{
  std::size_t operator()(glVec4 const & vertex) const noexcept
  {
    std::size_t result = 0;
    Core::utils::hash_combine(result, vertex.x);
    Core::utils::hash_combine(result, vertex.y);
    Core::utils::hash_combine(result, vertex.z);
    Core::utils::hash_combine(result, vertex.w);
    return result;
  }
};
