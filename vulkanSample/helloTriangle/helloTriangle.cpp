#include "helloTriangle.h"
#include <iostream>
#include <algorithm>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* user_data)
{
	std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void HelloTriangle::initWindow()
{
#if defined(WIN32)
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // create context yourself
	window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
#endif
}

void HelloTriangle::initVulkan()
{
	createInstance();
	createWindowSurface();
	createDevice();
	createSwapChain();
}

void HelloTriangle::createInstance()
{
	getLayersAndExtensions();

	VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.pEngineName = "JEngine";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = context.requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = context.requiredExtensions.data();
	createInfo.enabledLayerCount = context.requiredLayers.size();
	createInfo.ppEnabledLayerNames = context.requiredLayers.data();

	// using validation layers during instance creation
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData = nullptr;

	createInfo.pNext = &debugCreateInfo;

	VK_CHECK(vkCreateInstance(&createInfo, nullptr, &context.instance));

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");

	VK_CHECK(func(context.instance, &debugCreateInfo, nullptr, &context.debugMessenger));
}

void HelloTriangle::getLayersAndExtensions()
{
	uint32_t instanceExtensionCnt = 0;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCnt, nullptr));

	std::vector<VkExtensionProperties> availableExtensions(instanceExtensionCnt);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCnt, availableExtensions.data()));

	context.requiredExtensions = { 
		VK_KHR_SURFACE_EXTENSION_NAME, 
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#if defined(WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
	};

	bool ret = validateExtensions(context.requiredExtensions, availableExtensions);
	if (!ret)
	{
		throw std::runtime_error("Required extensions are not supported by the Vulkan implementation.");
		return;
	}

	uint32_t instanceLayerCnt = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCnt, nullptr));
	std::vector<VkLayerProperties> availableLayers(instanceLayerCnt);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCnt, availableLayers.data()));

	context.requiredLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	ret = validateLayers(context.requiredLayers, availableLayers);
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

void HelloTriangle::createWindowSurface()
{
#if defined(WIN32)
	if (glfwCreateWindowSurface(context.instance, window, nullptr, &context.surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Faield to create window surface");
	}
#endif
}

void HelloTriangle::createDevice()
{
	uint32_t physicalDeviceCnt = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCnt, nullptr));

	if (physicalDeviceCnt < 1)
	{
		throw std::runtime_error("Failed to find physical device");
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCnt);

	VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCnt, physicalDevices.data()));

	for (size_t i = 0; i < physicalDeviceCnt; ++i)
	{
		auto currentDevice = physicalDevices[i];

		uint32_t queueCnt = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(currentDevice, &queueCnt, nullptr);

		if (queueCnt < 1)
		{
			throw std::runtime_error("Failed to find valid queue family");
		}

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCnt);

		vkGetPhysicalDeviceQueueFamilyProperties(currentDevice, &queueCnt, queueFamilyProperties.data());

		for (size_t i = 0; i < queueCnt; ++i)
		{
			const auto& queueProp = queueFamilyProperties[i];

			VkBool32 surfacePresentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(currentDevice, i, context.surface, &surfacePresentSupport);

			if (surfacePresentSupport && queueProp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				context.physicalDevice = currentDevice;
				context.graphicsQueueIndex = i;
				break;
			}
		}

		if (context.graphicsQueueIndex < 0)
		{
			throw std::runtime_error("Failed to find a valid queue family");
		}

		uint32_t deviceExtensionCnt = 0;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(context.physicalDevice, nullptr, &deviceExtensionCnt, nullptr));
		std::vector<VkExtensionProperties> deviceExtensions;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(context.physicalDevice, nullptr, &deviceExtensionCnt, deviceExtensions.data()));

		std::vector<const char*> requiredDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		if (validateExtensions(requiredDeviceExtensions, deviceExtensions))
		{
			throw std::runtime_error("Failed to support device level extensions");
		}

		// create logical device with related queues
		float queuePriorities = 1.f;
		VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = context.graphicsQueueIndex;
		queueCreateInfo.pQueuePriorities = &queuePriorities;

		VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

		VK_CHECK(vkCreateDevice(context.physicalDevice, &deviceCreateInfo, nullptr, &context.logicalDevice));

		vkGetDeviceQueue(context.logicalDevice, context.graphicsQueueIndex, 0, &context.graphicsQueue);
	}
}

void HelloTriangle::createSwapChain()
{
	VkSurfaceCapabilitiesKHR surfaceProperties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physicalDevice, context.surface, &surfaceProperties));

	uint32_t formatCnt = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCnt, nullptr));
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCnt);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCnt, surfaceFormats.data()));
	VkSurfaceFormatKHR selectedFormat = surfaceFormats[0];
	for (const auto& availableFormat : surfaceFormats)
	{
		bool found = false;
		for (const auto& targetFormat : mPreferedFormats) 
		{
			if (availableFormat.format == targetFormat)
			{
				selectedFormat = availableFormat;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}

	uint32_t presentCnt = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentCnt, nullptr));
	std::vector<VkPresentModeKHR> presentModes(presentCnt);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentCnt, presentModes.data()));

	auto targetSwapchainImageCnt = surfaceProperties.minImageCount + 1;
	if (surfaceProperties.maxImageCount > 0 && surfaceProperties.maxImageCount < targetSwapchainImageCnt)
	{
		targetSwapchainImageCnt = surfaceProperties.maxImageCount;
	}

	VkExtent2D extent;
	if (surfaceProperties.currentExtent.width == 0xFFFFFFFF)
	{
		// according to window
#if defined(WIN32)
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		extent = { std::clamp(static_cast<uint32_t>(width), surfaceProperties.minImageExtent.width, surfaceProperties.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), surfaceProperties.minImageExtent.height, surfaceProperties.maxImageExtent.height) };
#endif
	}
	else
	{
		extent = surfaceProperties.currentExtent;
	}




	VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	swapchainCreateInfo.surface = context.surface;
	// swapchainCreateInfo.
}
