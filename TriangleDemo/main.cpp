#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vulkan.h>

//This Code is based on tutorial from https://vulkan-tutorial.com but using SDL2 rather than GLFW

/*
#define GLM_FORCE_CXX03 //This alongside disabling language extensions stops a bunch of L4 warnings about GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
*/
#include <iostream>
#include <string>
#include <vector>


class App
{
public:
	void start()
	{
		init();
		mainLoop();
		cleanUp();
	}

	static const char* getVulkanResultString(VkResult result)
	{
		switch ((int)result)
		{
		case VK_SUCCESS:
			return "VK_SUCCESS";
		case VK_NOT_READY:
			return "VK_NOT_READY";
		case VK_TIMEOUT:
			return "VK_TIMEOUT";
		case VK_EVENT_SET:
			return "VK_EVENT_SET";
		case VK_EVENT_RESET:
			return "VK_EVENT_RESET";
		case VK_INCOMPLETE:
			return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:
			return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL:
			return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:
			return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
			return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
		case VK_ERROR_INVALID_SHADER_NV:
			return "VK_ERROR_INVALID_SHADER_NV";
		case VK_RESULT_MAX_ENUM:
		case VK_RESULT_RANGE_SIZE:
			break;
		}
		if (result < 0)
			return "VK_ERROR_<Unknown>";
		return "VK_<Unknown>";
	}

	const int WIDTH = 800;
	const int HEIGHT = 600;

private:

	SDL_Window* window;

	VkInstance vulkanInst;
	
	void init()
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

		window = SDL_CreateWindow("Vulkan Testing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);

		createInstance();
	}

	void createInstance()
	{
		std::cout << "Creating Vulkan Instance:" << std::endl;

		listAvailableVulkanExts();

		//Generic App Info
		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Testing";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Vulkan Testing";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		appInfo.pNext = nullptr;


		//SDL Vulkan Specific
		unsigned int extensionCount;
		
		//Get required Extension Count for the current platform
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL))
		{
			std::cout << " - SDL Error: Couldn't fetch number of required extensions" << std::endl;
		}

		//Allocate Memory for names of these extensions 
		const char** extensionNames = (const char**)malloc(sizeof(const char*) * extensionCount);
		if (extensionNames == nullptr)
		{
			std::cout << " - Failed to malloc space for the extension names array" << std::endl;
		}

		//Get the names of the extensions
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames))
		{
			std::cout << " - SDL Error: Couldn't fetch names of required extensions" << std::endl;
		}


		std::cout << " - Required Extension Count: " << extensionCount << std::endl;
		std::cout << " - Extension Names:" << std::endl;

		for (unsigned int i = 0; i < extensionCount; i++)
		{
			std::cout << "   - " << extensionNames[i] << std::endl;
		}
		


		VkInstanceCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = extensionCount;
		createInfo.ppEnabledExtensionNames = extensionNames;
		createInfo.flags = 0;

		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.pNext = nullptr;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInst);
		if (result != VK_SUCCESS)
		{
			std::cout << " - Create Instance Failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			throw std::runtime_error("Failed to create instance!");
		}

		//Clear up malloc'd memory
		free(extensionNames);

		std::cout << "Create Instance Successful" << std::endl;
	}

	void listAvailableVulkanExts()
	{
		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << " - Available Extensions:" << std::endl;

		for (VkExtensionProperties extension : extensions) 
		{
			std::cout << "   - " << extension.extensionName << std::endl;
		}
	}

	void mainLoop()
	{
		bool go = true;
		while (go)
		{
			go = eventLoop();


		}
	}

	bool eventLoop()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				return false;
				break;

			case SDL_KEYUP:


				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					return false;
					break;
				}
				break;

			case SDL_KEYDOWN:

				break;

			}
		}

		return true;
	}

	void cleanUp()
	{
		vkDestroyInstance(vulkanInst, nullptr);

		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};


int main(int, char*[]) 
{
	App app;

	try {
		app.start();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}