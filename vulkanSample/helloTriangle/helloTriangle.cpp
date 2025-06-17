#include "helloTriangle.h"

void HelloTriangle::createInstance()
{
	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"Hello Triangle",
		VK_MAKE_VERSION(0, 0, 1), // application version
		"Jason Engine",
		VK_MAKE_VERSION(0, 0, 1), // engine version
		VK_API_VERSION_1_0 
	};

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
		nullptr, // pNext
		0,										// flags
		nullptr,								// pApplicationInfo
		// layers and extensions
	};
}

void HelloTriangle::getLayersAndExtensions()
{
	uint32_t instanceExtensionCnt = 0;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCnt, nullptr));

	std::vector<VkExtensionProperties> availableExtensions(instanceExtensionCnt);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCnt, availableExtensions.data()));

	std::vector<const char*> requiredExtensions{ 
		VK_KHR_SURFACE_EXTENSION_NAME, 
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#if defined(WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
	};

	bool ret = validateExtensions(requiredExtensions, availableExtensions);
	if (!ret)
	{
		throw std::runtime_error("Required extensions are not supported by the Vulkan implementation.");
		return;
	}

	uint32_t instanceLayerCnt = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCnt, nullptr));
	std::vector<VkLayerProperties> availableLayers(instanceLayerCnt);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCnt, availableLayers.data()));

	std::vector<const char*> requiredLayers{
		"VK_LAYER_KHRONOS_validation"
	};

	ret = validateLayers(requiredLayers, availableLayers);
	if (!ret)
	{
		throw std::runtime_error("Failed to support all required validation layers.");
	}

}

bool HelloTriangle::validateExtensions(const std::vector<const char*>& required, const std::vector<VkExtensionProperties>& availableExtensions)
{
	for (const auto& requiredExt : required)
	{
		bool found = false;
		for (const auto& ext : availableExtensions)
		{
			if (strcmp(requiredExt, ext.extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return false;
		}
	}
	return true;
}

bool HelloTriangle::validateLayers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& availableLayers)
{
	for (const auto& layer : required)
	{
		bool found = false;
		for (const auto& layerProp : availableLayers)
		{
			if (strcmp(layerProp.layerName, layer) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}