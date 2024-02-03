#include <memory>
#include <string_view>
#include <vector>

#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif
#endif

#include "../../Core/Logging.hpp"
#include "Renderer.hpp"
#include "VulkanRendering.hpp"

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

static std::list<VulkanContext> st_contexts; ///< each context for each GPU

// ------------------------ API implementation ----------------------------

void InitRenderingSystem(const usRenderingOptions & opts)
{
  io::Log(US_LOG_INFO, 0, "Initialize Vulkan");
  auto && ctx = st_contexts.emplace_back(opts);
  io::Log(US_LOG_INFO, 0, "Vulkan initialized successfully");
}

/// @brief clear all resources for rendering system
void TerminateRenderingSystem()
{
  io::Log(US_LOG_INFO, 0, "Destroy Vulkan");
  st_contexts.clear();
  io::Log(US_LOG_INFO, 0, "Vulkan has destroyed");
}

void Render2D(const RenderableScene & scene)
{
  for (auto && ctx : st_contexts)
    ctx.GetRenderer()->Render();
}

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

  renderer = std::make_unique<Renderer2D>(*this, surface);
}

VulkanContext::~VulkanContext() noexcept
{
  vkDeviceWaitIdle(device);
  renderer.reset();

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

inline vkb::Instance CreateInstance()
{
  vkb::Instance result;
  vkb::InstanceBuilder builder;
  auto inst_ret = builder.set_app_name("Example Vulkan Application")
                    .request_validation_layers()
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

inline vkb::PhysicalDevice SelectPhysicalDevice(vkb::Instance inst, VkSurfaceKHR surface)
{
  vkb::PhysicalDeviceSelector selector{inst};
  auto phys_ret = selector.set_surface(surface)
                    .require_present(surface != VK_NULL_HANDLE)
                    .set_desired_version(1, 3)
                    .select();

  if (!phys_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to select Vulkan Physical Device - %s",
            phys_ret.error().message());
    return vkb::PhysicalDevice{};
  }
  return phys_ret.value();
}

inline vk::SurfaceKHR CreateSurface(vkb::Instance inst, const usRenderingOptions & opts)
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
