#pragma once
#if defined(WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
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

	VkSurfaceKHR surface = VK_NULL_HANDLE;

	std::vector<const char*> requiredLayers;

	std::vector<const char*> requiredExtensions;

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	int32_t graphicsQueueIndex = -1;

	VkDevice logicalDevice = VK_NULL_HANDLE;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
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
	void createWindowSurface();
	void createDevice();
	void createSwapChain();

private:
	Context context;
#if defined(WIN32)
	GLFWwindow* window;
#endif
	int width{ 1920 };
	int height{ 1080 };

	const std::vector<VkFormat> mPreferedFormats{ VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_A8B8G8R8_SRGB_PACK32 };
};