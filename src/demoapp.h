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
#include "pipeline.h"


namespace VulkanDemo
{


class DemoApp
{
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const uint32_t PARTICLE_COUNT  = 512;
    const bool DRAW_PARTICLES = true;
    const bool DRAW_MESH = true;

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
    VkSwapchainKHR m_swapChain;                         // swap chain: series of images enqueued to the presentation engine
    std::vector<VkImage> m_swapChainImages;             // handles of the VkImage
    VkFormat m_swapChainImageFormat;                    // format chosen for the swap chain images
    VkExtent2D m_swapChainExtent;                       // extent chosen for the swap chain images
    std::vector<VkImageView> m_swapChainImageViews;     // image views
    std::vector<VkFramebuffer> m_swapChainFramebuffers; // framebuffers
    VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT; // nb of samples per pixel

    Pipeline m_graphicsPipeline_mesh;       // render and graphics pipeline for mesh
    Pipeline m_graphicsPipeline_particles;  // render and graphics pipeline for particles
    Pipeline m_computePipeline;             // compute shader pipeline
    bool m_useDepthBuffer = true;
    bool m_useColorAttachmentResolve = true;   

    // images
    Image m_textureImage;   // texture
    Image m_depthImage;     // depth buffer
    Image m_colorImage;     // image to store the desired number of samples per pixel

    // Semaphores and fences (for each in-flight frame)
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkSemaphore> m_computeFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_computeInFlightFences;

    // Resize flag
    bool m_framebufferResized = false;

    double m_lastFrameTime = 0.0;
    double m_lastTime = 0.0;

    // id of current frame to draw
    uint32_t m_currentFrame = 0;

    // Mesh contains vertex buffer and index buffer
    Mesh m_mesh;

    UniformBufferObject_graphics m_ubo_graphics{};
    UniformBufferObject_compute m_ubo_compute{};
    glm::mat4 m_initModel;
    GLtools::Camera m_camera;
    GLtools::Trackball m_trackball;

    // uniforms storage
    std::vector<VkBuffer> m_uniformBuffers_graphics;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory_graphics;
    std::vector<void*> m_uniformBuffersMapped_graphics;
    std::vector<VkBuffer> m_uniformBuffers_compute;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory_compute;
    std::vector<void*> m_uniformBuffersMapped_compute;
    std::vector<VkBuffer> m_computeShaderStorageBuffers;
    std::vector<VkDeviceMemory> m_computeShaderStorageBuffersMemory;

    std::default_random_engine rndEngine = std::default_random_engine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
 

    // main steps of run()
    void initWindow();
    void initVulkan();
    void initUBO();
    void mainLoop();
    void cleanup();

    // main steps of initVulkan()
    void pickPhysicalDevice();
    void createSwapChain();
    void createComputeShaderStorageBuffers();
    void createImageViews();
    void createRenderPasses();
    void createDescriptorSetLayouts();
    void createPipelines();
    void createFramebuffers();
    void createDepthResources();
    void createColorResources();
    void createUniformBuffers();
    void createDescriptorPools();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    // used in createSwapChain()
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);

    // used in createImageViews()
    VkImageView createImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels);

    // used in createDepthResources()
    VkFormat findSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features);
    VkFormat findDepthFormat();

    // main step of mainLoop()
    void drawFrame();

    // used in drawFrame()
    void recordGraphicsCommandBuffer(Pipeline _graphicsPipeline, 
                                     VkBuffer _vertexBuffer, VkBuffer _indexBuffer, 
                                     uint32_t _vertexCount, uint32_t _imageIndex);
    void recordComputeCommandBuffer(VkCommandBuffer _commandBuffer);
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