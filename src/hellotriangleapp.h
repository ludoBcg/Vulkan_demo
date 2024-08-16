/*********************************************************************************************************************
 *
 * hellotriangleapp.h
 *
 * The Vulkan application
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "utils.h"

namespace VulkanDemo
{

class HelloTriangleApp
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

public:

    void run();

private:

    GLFWwindow* m_window;
    VkInstance m_instance;                          
    VkDebugUtilsMessengerEXT m_debugMessenger;          // debug callback
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // the graphic device
    VkDevice m_device;                                  // logical device handle (i.e., similar to OpenGL context)
    VkQueue m_graphicsQueue;                            // graphics queue handle
    VkSurfaceKHR m_surface;                             // abstract type of surface to present rendered images to
    VkQueue m_presentQueue;                             // presentation queue handle
    VkSwapchainKHR m_swapChain;                         // swap chain
    std::vector<VkImage> m_swapChainImages;             // handles of the VkImage
    VkFormat m_swapChainImageFormat;                    // format chosen for the swap chain images
    VkExtent2D m_swapChainExtent;                       // extent chosen for the swap chain images
    std::vector<VkImageView> m_swapChainImageViews;     // image views
    VkRenderPass m_renderPass;                          // the render pipeline
    VkDescriptorSetLayout m_descriptorSetLayout;        // defines uniforms
    VkPipelineLayout m_pipelineLayout;                  // defines uniforms
    VkPipeline m_graphicsPipeline;                      // final graphics pipeline
    std::vector<VkFramebuffer> m_swapChainFramebuffers; // framebuffers
    VkCommandPool m_commandPool;                        // command pool
    VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT; // nb of samples per pixel

    // texture
    uint32_t m_mipLevels;
    VkImage m_textureImage;
    VkDeviceMemory m_textureImageMemory;
    VkImageView m_textureImageView;
    VkSampler m_textureSampler;

    // depth buffer
    VkImage m_depthImage;
    VkDeviceMemory m_depthImageMemory;
    VkImageView m_depthImageView;

    // image to store the desired number of samples per pixel
    VkImage m_colorImage;
    VkDeviceMemory m_colorImageMemory;
    VkImageView m_colorImageView;

    // Command buffer (for each in-flight frame)
    std::vector<VkCommandBuffer> m_commandBuffers;

    // Semaphores and fences (for each in-flight frame)
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    // Resize flag
    bool m_framebufferResized = false;

    // id of current frame to draw
    uint32_t m_currentFrame = 0;

    //// list of vertices
    //// 2  3 - 4
    //// | \ \  |
    //// |  \ \ |
    //// 1 - 0  5
    //const std::vector<Vertex> m_vertices = {
    //    // RGB triangle 
    //    { { 0.5f,  0.5f}, {1.0f, 0.0f, 0.0f} },
    //    { {-0.5f,  0.5f}, {0.0f, 1.0f, 0.0f} },
    //    { {-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} },

    //    // YCM triangle
    //    { {-0.5f, -0.5f}, {1.0f, 1.0f, 0.0f} },
    //    { { 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f} },
    //    { { 0.5f,  0.5f}, {1.0f, 0.0f, 1.0f} }
    //};

    // list of vertices
    // 2 -- 3
    // | \  |
    // |  \ |
    // 1 -- 0 
    const std::vector<Vertex> m_vertices_quads = {
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} }, // R
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} }, // G
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // B
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },  // W

        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} }, // R
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} }, // G
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // B
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }  // W
    };

    // list of indices
    const std::vector<uint16_t>  m_indices_quads = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    // Vertex buffer
    VkBuffer m_vertexBuffer;
    // Handle to the vertex buffer memory
    VkDeviceMemory m_vertexBufferMemory;

    // Index buffer
    VkBuffer m_indexBuffer;
    // Handle to the index buffer memory
    VkDeviceMemory m_indexBufferMemory;

    // uniforms storage
    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void*> m_uniformBuffersMapped;

    // Descriptors (i.e., uniforms)
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet>  m_descriptorSets;

    // main steps of run()
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    // main steps of initVulkan()
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void createColorResources();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();


    // used in pickPhysicalDevice()
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _device);
    bool isDeviceSuitable(VkPhysicalDevice _device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _device);

    // used in createSwapChain()
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);

    // used in createImageViews()
    VkImageView createImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels);

    // used in createGraphicsPipeline()
    VkShaderModule createShaderModule(const std::vector<char>& _code);

    // used in createDepthResources()
    VkFormat findSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat _format);

    // used in createTextureImage()
    void createImage(uint32_t _width, uint32_t _height, uint32_t _mipLevels, VkSampleCountFlagBits _numSamples, VkFormat _format,
                     VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties,
                     VkImage& _image, VkDeviceMemory& _imageMemory);
    void transitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels);
    void copyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height);
    void generateMipmaps(VkImage _image, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels);

    // used in createVertexBuffer()
    uint32_t findMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

    // main step of mainLoop()
    void drawFrame();

    // used in drawFrame()
    void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex);
    void cleanupSwapChain();
    void recreateSwapChain();
    void updateUniformBuffer(uint32_t _currentImage);

    // UI callbacks
    static void framebufferResizeCallback(GLFWwindow* _window, int _width, int _height);

    // buffer creation
    void createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties,
        VkBuffer& _buffer, VkDeviceMemory& _bufferMemory);
    // buffer copy
    void copyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer _commandBuffer);

    VkSampleCountFlagBits getMaxUsableSampleCount();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT _messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
        void* _pUserData);

};

} // namespace VulkanDemo