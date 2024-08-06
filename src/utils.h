/*********************************************************************************************************************
 *
 * utils.h
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <optional>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // handles data alignment automatically
#include <glm/glm.hpp>

//#include <vulkan/vulkan.h>  // included by GLFW/glfw3.h below

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // gives access to native platform functions


namespace VulkanDemo
{

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    /*
     * Structure for vertex attributes
     */
    struct Vertex 
    {
        glm::vec2 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() 
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            // VK_VERTEX_INPUT_RATE_VERTEX = Move to the next data entry after each vertex
            // VK_VERTEX_INPUT_RATE_INSTANCE = Move to the next data entry after each instance

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() 
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            // Attribute description for position
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            // Attribute description for color
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            // Attribute description for UVs
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }

    };


    /*
     * Structure to store data associated with vertex processing (i.e., MVP matrices)
     */
    struct UniformBufferObject 
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };


    // List of validation layers to enable
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // List of required device extensions
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME // swap chain is the equivalent of default framebuffer
    };


    // use validation layers in debug mode only
#ifdef NDEBUG // "not debug"
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


    /*
     * Checks if all of the requested layers are available
     */
    inline bool checkValidationLayerSupport()
    {
        // lists available layers
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        //check if all of the layers in validationLayers exist in the availableLayers
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break; // early exit
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        std::cout << " checkValidationLayerSupport(): OK " << std::endl;
        return true;
    }


    /*
     * Checks if all of the requested extensions are available
     */
    inline bool checkDeviceExtensionSupport(VkPhysicalDevice _device) 
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

        // list of available extensions
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

        // list of required extensions
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        // OK if all required extensions are available
        return requiredExtensions.empty();
    }


    /*
     * Returns the required list of extensions based on whether validation layers are enabled or not
     */
    inline std::vector<const char*> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // same as VK_EXT_debug_utils
        }

        std::cout << " getRequiredExtensions(): OK " << std::endl;
        return extensions;
    }


    /*
     * Used to load the vkCreateDebugUtilsMessengerEXT() funtion, 
     * which is not automatically loaded since it's an extension
     */
    inline  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }


    inline  void DestroyDebugUtilsMessengerEXT(VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }


    /*
     * Stores queue families
     */
    struct QueueFamilyIndices
    {
        // std::optional is a wrapper that contains no value until you assign something to it
        std::optional<uint32_t> graphicsFamily; // queue families supporting drawing commands
        std::optional<uint32_t> presentFamily;  // queue families supporting presentation 

        bool isComplete()
        {
            // check if graphicsFamily and presentFamily have a value
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };


    /*
     * Stores swap chains details to check compatibility with, e.g., window surface
     */
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    /*
     * Read shader files
     */
    static std::vector<char> readFile(const std::string& filename) 
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
        {
            throw std::runtime_error("failed to open file! Check if relative path to file is consistent with working directory");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }



} // namespace VulkanDemo
