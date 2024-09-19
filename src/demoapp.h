/*********************************************************************************************************************
 *
 * demoapp.h
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


#include "context.h"
#include "mesh.h"
#include "image.h"
#include "GLtools.h"

namespace VulkanDemo
{


class DemoApp
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

public:

    void run();

private:

    // Context contains handles for: 
    //  - VkInstance,
    //  - debug callback,
    //  - logical device, 
    //  - physical device,
    //  - command pool,
    //  - graphics queue,
    //  - presentation queue
    std::shared_ptr<Context> m_contextPtr = nullptr; 

    GLFWwindow* m_window;
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
    VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT; // nb of samples per pixel

    // images
    Image m_textureImage;   // texture
    Image m_depthImage;     // depth buffer
    Image m_colorImage;     // image to store the desired number of samples per pixel

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

    // Mesh contains vertex buffer and index buffer
    Mesh m_mesh;

    UniformBufferObject m_ubo{};
    glm::mat4 m_initModel;
    GLtools::Camera m_camera;
    GLtools::Trackball m_trackball;

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
    void initUBO();
    void mainLoop();
    void cleanup();

    // main steps of initVulkan()
    void pickPhysicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createDepthResources();
    void createColorResources();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();


    // used in pickPhysicalDevice()
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

    // main step of mainLoop()
    void drawFrame();

    // used in drawFrame()
    void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex);
    void cleanupSwapChain();
    void recreateSwapChain();
    void updateUniformBuffer(uint32_t _currentImage);

    // UI callbacks
    static void framebufferResizeCallback(GLFWwindow* _window, int _width, int _height);
    static void keyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
    static void mouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
    static void scrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    static void cursorPosCallback(GLFWwindow* _window, double _x, double _y);

    VkSampleCountFlagBits getMaxUsableSampleCount();


};

} // namespace VulkanDemo