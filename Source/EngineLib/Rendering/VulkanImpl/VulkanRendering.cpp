#include <memory>
#include <string_view>
#include <vector>

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include "../../Core/Logging.hpp"
#include "../RenderingSystem.hpp"


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

static vkb::Instance st_vulkanInstance;
static vkb::PhysicalDevice st_choosenGPU;

static inline bool InitInstance();
static inline bool InitPhysicalDevices();

struct VulkanContext
{
  VulkanContext(const usRenderingOptions & opts);
  ~VulkanContext();

  const vkb::DispatchTable * operator->() const {  return &dispatch_table; }

private:
  vkb::Device device;
  vk::Queue graphics_queue;
  vkb::DispatchTable dispatch_table;
};
static std::vector<VulkanContext> st_contexts;


void InitRenderingSystem(const usRenderingOptions & opts)
{
  io::Log(US_LOG_INFO, 0, "Initialize Vulkan");
  InitInstance();
  InitPhysicalDevices();

  io::Log(US_LOG_INFO, 0, "Vulkan initialized successfully");
}

/// @brief clear all resources for rendering system
void TerminateRenderingSystem()
{
  io::Log(US_LOG_INFO, 0, "Destroy Vulkan");
  st_contexts.clear();
  vkb::destroy_instance(st_vulkanInstance);
  io::Log(US_LOG_INFO, 0, "Vulkan has destroyed");
}

// -------------------------- Implementation -----------------------

VulkanContext::VulkanContext(const usRenderingOptions & opts)
{
  vkb::DeviceBuilder device_builder{st_choosenGPU};
  auto dev_ret = device_builder.build();
  if (!dev_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to create Vulkan device - %s", dev_ret.error().message());
  }
  device = dev_ret.value();
  dispatch_table = device.make_table();
  
  // Get the graphics queue with a helper function
  auto graphics_queue_ret = device.get_queue(vkb::QueueType::graphics);
  if (!graphics_queue_ret)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to get graphics queue - %s",
            graphics_queue_ret.error().message());
  }
  graphics_queue = graphics_queue_ret.value();
}

VulkanContext::~VulkanContext()
{
  vkb::destroy_device(device);
}


inline bool InitInstance()
{
  if (st_vulkanInstance.instance == VK_NULL_HANDLE)
  {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
                      .request_validation_layers()
                      .set_debug_callback(debugCallback)
                      .build();
    if (!inst_ret)
    {
      io::Log(US_LOG_ERROR, 0, "Failed to create Vulkan instance - %s", inst_ret.error().message());
      return false;
    }
    st_vulkanInstance = inst_ret.value();
    return true;
  }
}

inline bool InitPhysicalDevices()
{
  if (st_choosenGPU.physical_device == VK_NULL_HANDLE)
  {
    vkb::PhysicalDeviceSelector selector{st_vulkanInstance};
    auto phys_ret = selector.set_surface(VK_NULL_HANDLE)
                      .require_present(false)
                      .set_minimum_version(1, 1) // require a vulkan 1.1 capable device
                      .require_dedicated_transfer_queue()
                      .select();
    if (!phys_ret)
    {
      io::Log(US_LOG_ERROR, 0, "Failed to select Vulkan Physical Device - %s",
              phys_ret.error().message());
      return false;
    }
    st_choosenGPU = phys_ret.value();
    return true;
  }
}
