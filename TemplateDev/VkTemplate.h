#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace JEngine
{
	struct VulkanTemplateInitParam
	{
		std::vector<const char*> enabledExtensions;
		std::vector<const char*> enabledLayers;
	};

	class VulkanTemplate
	{
		public:
			void Init(const VulkanTemplateInitParam& initParam);

		private:
			void createInstance(const std::vector<const char*>& enabledExtensions, const std::vector<const char*>& enabledLayers);
			void pickPhysicalDevice();
			void createLogicalDevice();
			void createSurface();
			void createSwapchain();

		private:
			VkInstance mInstance;
			VkPhysicalDevice mPhysicalDevice;
			VkDevice mLogicalDevice;
			VkSurfaceKHR mSurface;
	};

}