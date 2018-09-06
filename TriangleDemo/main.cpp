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


	const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

private:

	SDL_Window* window;

	VkInstance vulkanInst;
	VkDebugUtilsMessengerEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	std::vector<const char*> getRequiredExt()
	{
		//SDL Vulkan Specific
		unsigned int extensionCount;

		//Get required Extension Count for the current platform
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL))
		{
			std::cout << " - SDL Error: Couldn't fetch number of required extensions" << std::endl;
		}

		//Other Extensions we need, mostly just going to be for debug purposes



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


		

		std::vector<const char*> extensions(extensionNames, extensionNames + extensionCount);
		
		//Clear up malloc'd memory
		free(extensionNames);

		//Enable Debug Callback Functions
		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		std::cout << " - Required Extension Count: " << extensions.size() << std::endl;
		std::cout << " - Extension Names:" << std::endl;


		for (unsigned int i = 0; i < extensions.size(); i++)
		{
			std::cout << "   - " << extensions[i] << std::endl;
		}


		return extensions;
	}

	void createInstance()
	{
		//Only for debug builds
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("Requested Validation Layers Not Available");
		}


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


		std::vector<const char*> extensions = getRequiredExt();

		VkInstanceCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = (uint32_t)extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.flags = 0;
		createInfo.pNext = nullptr;

		if (enableValidationLayers) 
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else 
		{
			createInfo.enabledLayerCount = 0;
		}
		

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInst);
		if (result != VK_SUCCESS)
		{
			std::cout << " - Create Instance Failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			throw std::runtime_error("Failed to create instance!");
		}

		

		std::cout << "Create Instance Successful" << std::endl;
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layerCount = 0;
		
		VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		if (result != VK_SUCCESS)
		{
			std::cout << "vkEnumerateInstanceLayer failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			return false;
		}

		std::vector<VkLayerProperties> availableLayers(layerCount);
		result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		if (result != VK_SUCCESS)
		{
			std::cout << "vkEnumerateInstanceLayer failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			return false;
		}


		//Looks if all requested layers are available
		for (const char* layerName : validationLayers) 
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) 
			{
				if (strcmp(layerName, layerProperties.layerName) == 0) 
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) 
			{
				return false;
			}
		}

		return true;
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

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
		VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* /*pUserData*/)
	{

		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	void setupDebugCallback() 
	{
		if (!enableValidationLayers) 
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
		createInfo.flags = 0; //Needs to be set due to Vulkan Spec

		
		if (CreateDebugUtilsMessengerEXT(vulkanInst, &createInfo, nullptr, &callback) != VK_SUCCESS) 
			throw std::runtime_error("failed to set up debug callback!");
		
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pCallback) 
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) 
		{
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else 
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance, 
		VkDebugUtilsMessengerEXT callback, 
		const VkAllocationCallbacks* pAllocator) 
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) 
			func(instance, callback, pAllocator);
		
	}


	void selectPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		VkResult result = vkEnumeratePhysicalDevices(vulkanInst, &deviceCount, nullptr);
		if (result != VK_SUCCESS)
		{
			std::cout << " - Create Instance Failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			throw std::runtime_error("Failed to create instance!");
		}

		if (deviceCount == 0)
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		
		std::vector<VkPhysicalDevice> devices(deviceCount);
		VkResult result = vkEnumeratePhysicalDevices(vulkanInst, &deviceCount, devices.data());
		if (result != VK_SUCCESS)
		{
			std::cout << " - Create Instance Failed, VkResult Code: " << getVulkanResultString(result) << std::endl;
			throw std::runtime_error("Failed to create instance!");
		}


		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	bool isDeviceSuitable(VkPhysicalDevice device) 
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;
	}

	void init()
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

		window = SDL_CreateWindow("Vulkan Testing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);

		createInstance();
		setupDebugCallback();
		selectPhysicalDevice();
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
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(vulkanInst, callback, nullptr);

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