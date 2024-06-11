#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication
{
public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	void initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan()
	{
		createInstance();
	}

	void createInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "Jason Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(availableExtensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, extensions.data());

		std::cout << "available extensions:\n";

		for (const auto& extension : extensions)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;

		if (checkExtensions(extensions, glfwExtensions, glfwExtensionCount))
			std::cout << "All required Extensions are found!" << std::endl;
		else
		{
			std::cout << "Missing Extensions!" << std::endl;
		}
		
		VkResult result = vkCreateInstance(&createInfo, nullptr, & instance);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vk Instance!");
		}
	}

	bool checkExtensions(const std::vector<VkExtensionProperties> availbleExt, const char** extNames, int extCount)
	{
		for (int i = 0; i < extCount; ++i)
		{
			auto targetExt = extNames[i];
			bool found = false;
			for (const auto& extProp : availbleExt)
			{
				if (strcmp(targetExt, extProp.extensionName))
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

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);

		glfwTerminate();
	}

private:
	GLFWwindow* window;
	VkInstance instance;
};

int main()
{
	HelloTriangleApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}