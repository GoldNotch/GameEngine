#include "../RenderingSystem.hpp"
#include <memory>
#include <vector>
#include <string_view>

#include <vulkan/vulkan.hpp>

static constexpr std::array st_requiredLayers = { "VK_LAYER_KHRONOS_validation" };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	Log(usLogStatus::ERROR, -1, "debug callback");
	//std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

struct VulkanApp
{
	VulkanApp();
private:
	void checkValidationLayers() const;
private:
	vk::ApplicationInfo appInfo;
	vk::InstanceCreateInfo createInfo;
	vk::Instance instance;

	std::vector<vk::ExtensionProperties> extensions;
	std::vector<vk::LayerProperties> layers;
};

VulkanApp::VulkanApp()
	: appInfo("USEngine", VK_MAKE_VERSION(1, 0, 0), "USEngine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0)
{
	layers = vk::enumerateInstanceLayerProperties();
	extensions = vk::enumerateInstanceExtensionProperties();
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	createInfo.ppEnabledLayerNames = st_requiredLayers.data();
	createInfo.enabledExtensionCount = 0;
	instance = vk::createInstance(createInfo);
}

void VulkanApp::checkValidationLayers() const
{
	for (auto&& validation_layer : st_requiredLayers)
	{
		if (std::find_if(layers.begin(), layers.end(),
			[&validation_layer](const vk::LayerProperties& layer)
			{return strcmp(layer.layerName, validation_layer) == 0; }) == layers.end())
			throw std::runtime_error("Required validation layer, but it's not available");
	}
}

static std::unique_ptr<VulkanApp> st_vkApp;


void InitRenderingSystem()
{
	st_vkApp = std::make_unique<VulkanApp>();
}


