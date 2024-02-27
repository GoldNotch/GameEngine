#include "VulkanContext.hpp"

#include <memory>
#include <string_view>
#include <vector>

#include <Formatter.hpp>
#include <Logging.hpp>

#include "MemoryManager.hpp"
#include "Renderer.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
{
  using SeverityBitFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
  using SeverityFlags = VkDebugUtilsMessageSeverityFlagBitsEXT;
  using TypeBitFlag = vk::DebugUtilsMessageTypeFlagBitsEXT;
  using TypeFlags = VkDebugUtilsMessageTypeFlagsEXT;
  if (messageSeverity == static_cast<SeverityFlags>(SeverityBitFlag::eError) ||
      messageType == static_cast<TypeFlags>(TypeBitFlag::eValidation))
    io::Log(US_LOG_ERROR, 0, pCallbackData->pMessage);
  else if (messageSeverity == static_cast<SeverityFlags>(SeverityBitFlag::eWarning) ||
           messageType == static_cast<TypeFlags>(TypeBitFlag::ePerformance))
    io::Log(US_LOG_WARNING, 0, pCallbackData->pMessage);
  else
    io::Log(US_LOG_INFO, 0, pCallbackData->pMessage);

  return VK_FALSE;
}

/// @brief initialize vulkan instance
vkb::Instance CreateInstance();
/// @brief choose and init physical device for vulkan
vkb::PhysicalDevice SelectPhysicalDevice(vkb::Instance inst, VkSurfaceKHR surface);
/// @brief platform specific function, creates surface for presentation
vk::SurfaceKHR CreateSurface(vkb::Instance inst, const usRenderingOptions & opts);

// -------------------------- Context Implementation -----------------------

VulkanContext::VulkanContext(const usRenderingOptions & opts)
{
  vulkan_instance = CreateInstance();
  auto surface = CreateSurface(vulkan_instance, opts);
  choosen_gpu = SelectPhysicalDevice(vulkan_instance, surface);
  vkb::DeviceBuilder device_builder{choosen_gpu};
  auto dev_ret = device_builder.build();
  if (!dev_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to create Vulkan device - %s", dev_ret.error().message());
  }
  device = dev_ret.value();
  dispatch_table = device.make_table();

  memory_manager = std::make_unique<MemoryManager>(*this);
  renderer = std::make_unique<Renderer>(*this, surface);
}

VulkanContext::~VulkanContext() noexcept
{
  vkDeviceWaitIdle(device);
  renderer.reset();
  memory_manager.reset();
  vkb::destroy_device(device);
  vkb::destroy_instance(vulkan_instance);
}

std::pair<uint32_t, vk::Queue> VulkanContext::GetQueue(vkb::QueueType type) const
{
  auto queue_ret = device.get_queue(type);
  auto familly_index = device.get_queue_index(type);
  if (!queue_ret)
    throw std::runtime_error("failed to request queue");
  return std::make_pair(familly_index.value(), queue_ret.value());
}

vk::Semaphore VulkanContext::CreateVkSemaphore() const
{
  VkSemaphoreCreateInfo info{};
  VkSemaphore result = VK_NULL_HANDLE;
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  // Don't use createSemaphore in dispatchTable because it's broken
  if (vkCreateSemaphore(device, &info, nullptr, &result) != VK_SUCCESS)
    throw std::runtime_error("failed to create semaphore");
  return vk::Semaphore(result);
}

vk::Fence VulkanContext::CreateFence(bool locked) const
{
  VkFenceCreateInfo info{};
  VkFence result = VK_NULL_HANDLE;
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  if (locked)
    info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  // Don't use createFence in dispatchTable because it's broken
  if (vkCreateFence(device, &info, nullptr, &result) != VK_SUCCESS)
    throw std::runtime_error("failed to create fence");
  return vk::Fence(result);
}

vk::CommandPool VulkanContext::CreateCommandPool(uint32_t queue_family_index) const
{
  VkCommandPool commandPool = VK_NULL_HANDLE;

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queue_family_index;
  // Don't use createSemaphore in dispatchTable because it's broken
  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create command pool!");
  }
  return vk::CommandPool{commandPool};
}

vk::CommandBuffer VulkanContext::CreateCommandBuffer(vk::CommandPool pool) const
{
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate command buffers!");

  return vk::CommandBuffer{commandBuffer};
}

// --------------------- Static functions ------------------------------

vkb::Instance CreateInstance()
{
  vkb::Instance result;
  vkb::InstanceBuilder builder;
  auto inst_ret = builder
                    .set_app_name("Example Vulkan Application")
                    //.request_validation_layers()
                    .set_debug_callback(debugCallback)
                    .build();
  if (!inst_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to create Vulkan instance - %s", inst_ret.error().message());
  }
  else
    result = inst_ret.value();
  return result;
}

vkb::PhysicalDevice SelectPhysicalDevice(vkb::Instance inst, VkSurfaceKHR surface)
{
  vkb::PhysicalDeviceSelector selector{inst};
  auto phys_ret = selector.set_surface(surface)
                    .require_present(surface != VK_NULL_HANDLE)
                    .set_minimum_version(1, 3)
                    .select();

  if (!phys_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to select Vulkan Physical Device - %s",
            phys_ret.error().message());
    return vkb::PhysicalDevice{};
  }
  return phys_ret.value();
}

vk::SurfaceKHR CreateSurface(vkb::Instance inst, const usRenderingOptions & opts)
{
  VkSurfaceKHR surface = VK_NULL_HANDLE;
#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
  VkWin32SurfaceCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.hwnd = opts.hWindow;
  createInfo.hinstance = opts.hInstance;

  if (vkCreateWin32SurfaceKHR(inst, &createInfo, nullptr, &surface) != VK_SUCCESS)
    throw std::runtime_error("failed to create window surface!");
#else

#endif
#endif
  return vk::SurfaceKHR(surface);
}
