#pragma once
#if defined(WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <vector>

#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			throw std::runtime_error("Detected Vulkan error: " + std::to_string(err)); \
		}                                                                              \
	} while (0)


struct Context
{
	VkInstance instance = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
};


class HelloTriangle
{
public:
	HelloTriangle();

private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanResource();

	// Util Function
	void createInstance();
	void createPhysicalDevice();
	void createLogicalDevice();
	void getLayersAndExtensions();
	bool validateExtensions(const std::vector<const char*> &required, const std::vector<VkExtensionProperties> &availableExtensions);
	bool validateLayers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& availableLayers);

private:
	Context context;
};