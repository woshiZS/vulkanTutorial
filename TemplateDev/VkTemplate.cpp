#include "VkTemplate.h"
#include <stdexcept>

namespace JEngine
{
	void VulkanTemplate::Init(const VulkanTemplateInitParam& initParam)
	{
		createInstance(initParam.enabledExtensions, initParam.enabledExtensions);
		pickPhysicalDevice();
		createLogicalDevice();
		createSurface();
		createSwapchain();
	}

	void VulkanTemplate::createInstance(const std::vector<const char*>& enabledExtensions, const std::vector<const char*>& enabledLayers)
	{
		VkApplicationInfo appInfo
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "JEngine Application",
			.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName = "JEngine",
			.engineVersion = VK_MAKE_VERSION(0, 0, 1),
			.apiVersion = VK_API_VERSION_1_0,
		};

		VkInstanceCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
			.ppEnabledLayerNames = enabledLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
			.ppEnabledExtensionNames = enabledExtensions.data(),
		};

		VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan instance!");
		}
	}
}