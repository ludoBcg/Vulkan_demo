/*********************************************************************************************************************
 *
 * demoapp.cpp
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#define NOMINMAX 
#include <cstdint> // Necessary for uint32_t
//#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
//#include <chrono>
#include <unordered_map>


#include "demoapp.h"


namespace VulkanDemo
{


/*
 * Main app execution
 */
void DemoApp::run()
{
    initWindow();
    initVulkan();
    initUBO();
    mainLoop();
    cleanup();
}


/*
 * Creates a GLFW window
 */
void DemoApp::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // setup glfw window
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan_demo", nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);

    infoLog() << "initWindow(): OK ";
}

/*
 * Initializes Vulkan 
 */
void DemoApp::initVulkan()
{
    m_contextPtr = std::make_shared<Context>();

    m_contextPtr->createInstance();
    m_contextPtr->setupDebugMessenger();
    m_contextPtr->createSurface(m_window);
    pickPhysicalDevice();
    m_contextPtr->createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPasses();
    createDescriptorSetLayouts();
    createPipelines();
    m_contextPtr->createCommandPool();
    createColorResources();
    createDepthResources();
    createFramebuffers();
    createComputeShaderStorageBuffers();
    m_textureImage.createTextureImage(*m_contextPtr);
    m_textureImage.createTextureImageView(*m_contextPtr);
    m_textureImage.createTextureSampler(*m_contextPtr);
    m_mesh.loadModel();
    m_mesh.createVertexBuffer(*m_contextPtr);
    m_mesh.createIndexBuffer(*m_contextPtr);
    createUniformBuffers();
    createDescriptorPools();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();

    infoLog() << "initVulkan(): OK ";
}

/*
 * Initializes transformation Matrices 
 */
void DemoApp::initUBO()
{
    m_camera.init(0.01f, 8.0f, 45.0f, 1.0f, m_swapChainExtent.width, m_swapChainExtent.height, glm::vec3(0.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0); 
    m_trackball.init(m_swapChainExtent.width, m_swapChainExtent.height);

    // initial transformation to re-orient mesh
    m_initModel = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // build MVP matrices
    m_ubo.model = m_initModel;
    m_ubo.view = m_camera.getViewMatrix();
    m_ubo.proj = m_camera.getProjectionMatrix();
    m_ubo.proj[1][1] *= -1;
    m_ubo.lightPos = glm::vec3(2.0f, 2.0f, 0.0f); // light source position in view space

    m_ubo.deltaTime = static_cast<float>(m_lastFrameTime) * 2.0f;
}


/*
 * Executes main loop until app closed
 */
void DemoApp::mainLoop()
{
    infoLog() << "enter main loop ";
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        drawFrame();
    }

    vkDeviceWaitIdle(m_contextPtr->getDevice());

    infoLog() << "exit main loop ";
}

/*
 * Cleanup before closing
 */
void DemoApp::cleanup()
{
    cleanupSwapChain();

    m_textureImage.cleanup(*m_contextPtr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroyBuffer(m_contextPtr->getDevice(), m_uniformBuffers.at(i), nullptr);
        vkFreeMemory(m_contextPtr->getDevice(), m_uniformBuffersMemory.at(i), nullptr);

        vkDestroyBuffer(m_contextPtr->getDevice(), m_computeShaderStorageBuffers.at(i), nullptr);
        vkFreeMemory(m_contextPtr->getDevice(), m_computeShaderStorageBuffersMemory.at(i), nullptr);
    }

    m_mesh.cleanup(*m_contextPtr);

    m_graphicsPipeline_mesh.destroy(m_contextPtr->getDevice(), m_contextPtr->getCommandPool());
    m_graphicsPipeline_particles.destroy(m_contextPtr->getDevice(), m_contextPtr->getCommandPool());
    m_computePipeline.destroy(m_contextPtr->getDevice(), m_contextPtr->getCommandPool());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroySemaphore(m_contextPtr->getDevice(), m_imageAvailableSemaphores.at(i), nullptr);
        vkDestroySemaphore(m_contextPtr->getDevice(), m_renderFinishedSemaphores.at(i), nullptr);
        vkDestroySemaphore(m_contextPtr->getDevice(), m_computeFinishedSemaphores.at(i), nullptr);
        vkDestroyFence(m_contextPtr->getDevice(), m_inFlightFences.at(i), nullptr);
        vkDestroyFence(m_contextPtr->getDevice(), m_computeInFlightFences.at(i), nullptr);
    }

    // Command buffers are automatically freed when their command pool is destroyed
    vkDestroyCommandPool(m_contextPtr->getDevice(), m_contextPtr->getCommandPool(), nullptr);

    vkDestroyDevice(m_contextPtr->getDevice(), nullptr);

    if (enableValidationLayers) 
    {
        DestroyDebugUtilsMessengerEXT(m_contextPtr->getInstance(), m_contextPtr->getDebugMessenger(), nullptr);
    }

    vkDestroySurfaceKHR(m_contextPtr->getInstance(), m_contextPtr->getSurface(), nullptr);

    vkDestroyInstance(m_contextPtr->getInstance(), nullptr);

    glfwDestroyWindow(m_window);

    glfwTerminate();

    infoLog() << "cleanup(): OK ";
}


/*
 * Device selection
 */
void DemoApp::pickPhysicalDevice()
{
    // select a graphics card in the system that supports the features we need

    // lists the graphics cards 
    //  starts with querying just the number.
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_contextPtr->getInstance(), &deviceCount, nullptr);

    // If there are 0 devices with Vulkan support then there is no point going further.
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // allocate an array to hold all of the VkPhysicalDevice handles.
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_contextPtr->getInstance(), &deviceCount, devices.data());


    // check if any of the physical devices meet the requirements defined in isDeviceSuitable()
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device, m_contextPtr->getSurface()))
        {
            m_contextPtr->setPhysicalDevice(device);
            m_msaaSamples = m_useColorAttachmentResolve ? getMaxUsableSampleCount() : VK_SAMPLE_COUNT_1_BIT;
            break; // early exit
        }
    }

    if (m_contextPtr->getPhysicalDevice() == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    infoLog() << "pickPhysicalDevice(): OK ";
}


/*
 * Surface format (color depth)
 */
VkSurfaceFormatKHR DemoApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
{
    for (const auto& availableFormat : _availableFormats) 
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }
    return _availableFormats.at(0);
}


/*
 * Presentation mode (conditions for "swapping" images to the screen)
 */
VkPresentModeKHR DemoApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes)
{
    for (const auto& availablePresentMode : _availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}


/*
 * Swap extent (resolution of images in swap chain)
 */
VkExtent2D DemoApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities) 
{
    if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return _capabilities.currentExtent;
    }
    else 
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = 
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


/*
 * Creation of swap chain
 */
void DemoApp::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_contextPtr->getPhysicalDevice(), m_contextPtr->getSurface());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_contextPtr->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(m_contextPtr->getPhysicalDevice(), m_contextPtr->getSurface());
    std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsAndComputeFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_contextPtr->getDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_contextPtr->getDevice(), m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_contextPtr->getDevice(), m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;

    initUBO(); // re-init camera and trackball when resize occurs

    infoLog() << "createSwapChain(): OK ";
}


/*
 * Creation of one image view
 */
VkImageView DemoApp::createImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = _aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = _mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(m_contextPtr->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


/*
 * Creation of image views
 */
void DemoApp::createImageViews() 
{
    // creates as many image views as we have images
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for (uint32_t i = 0; i < m_swapChainImages.size(); i++) 
    {
        m_swapChainImageViews.at(i) = createImageView(m_swapChainImages.at(i), m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    infoLog() << "createImageViews(): OK ";
}


/*
 * Creation of render pass 
 */
void DemoApp::createRenderPasses()
{
    bool clearBuffers = (DRAW_PARTICLES && DRAW_MESH) ? false : true;
    m_graphicsPipeline_particles.createRenderPass(m_contextPtr->getDevice(), m_swapChainImageFormat, m_useDepthBuffer, m_useColorAttachmentResolve, true, m_msaaSamples, findDepthFormat()/*VK_FORMAT_D32_SFLOAT*/);
    m_graphicsPipeline_mesh.createRenderPass(m_contextPtr->getDevice(), m_swapChainImageFormat, m_useDepthBuffer, m_useColorAttachmentResolve, clearBuffers, m_msaaSamples, findDepthFormat()); 

    infoLog() << "createRenderPasses(): OK ";
}


/*
 * Bindings layouts
 */
void DemoApp::createDescriptorSetLayouts()
{
    // 1. Graphics pipeline Descriptors
    m_graphicsPipeline_mesh.createGraphicsDescriptorSetLayout( m_contextPtr->getDevice());
    m_graphicsPipeline_particles.createGraphicsDescriptorSetLayout( m_contextPtr->getDevice());

    // 2. Compute pipeline Descriptors
    m_computePipeline.createComputeDescriptorSetLayout( m_contextPtr->getDevice());

    infoLog() << "createComputeDescriptorSetLayout(): OK ";
}


/*
 * Creation of graphics pipeline
 */
void DemoApp::createPipelines()
{
    // 1. Graphics pipeline

    auto vertShaderCode = GLtools::readFile("../src/shaders/vert.spv");
    auto fragShaderCode = GLtools::readFile("../src/shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(m_contextPtr->getDevice(), vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(m_contextPtr->getDevice(), fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

    // describes the format of the vertex data that will be passed to the vertex shader
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    Vertex::getAttributeDescriptions(attributeDescriptions);
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


    m_graphicsPipeline_mesh.createGraphicsPipeline( m_contextPtr->getDevice(),
                                                    vertShaderModule, fragShaderModule,
                                                    vertexInputInfo,
                                                    m_useDepthBuffer, m_msaaSamples, 0, 3, 0 );



    // 2.5. Graphics pipleine particles
    // create graphics pipeline

    vertShaderCode = GLtools::readFile("../src/shaders/vert_particles.spv");
    fragShaderCode = GLtools::readFile("../src/shaders/frag_particles.spv");

    vertShaderModule = createShaderModule(m_contextPtr->getDevice(), vertShaderCode);
    fragShaderModule = createShaderModule(m_contextPtr->getDevice(), fragShaderCode);

    bindingDescription = Particle::getBindingDescription();
    attributeDescriptions.clear();
    Particle::getAttributeDescriptions(attributeDescriptions);
    // describes the format of the vertex data that will be passed to the vertex shader
    vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    m_graphicsPipeline_particles.createGraphicsPipeline( m_contextPtr->getDevice(),
                                               vertShaderModule, fragShaderModule,
                                               vertexInputInfo,/* nullptr,*/
                                               m_useDepthBuffer, m_msaaSamples, 0, 0, 2 );

    // 2. Compute pipeline
    
    auto computeShaderCode = GLtools::readFile("../src/shaders/comp.spv");

    VkShaderModule computeShaderModule = createShaderModule(m_contextPtr->getDevice(), computeShaderCode);

    m_computePipeline.createComputePipeline(m_contextPtr->getDevice(),
                                            computeShaderModule/*,
                                            m_computeDescriptorSetLayout*/ );

    infoLog() << "createPipelines(): OK ";
}


/*
 * Creation of framebuffers
 */
void DemoApp::createFramebuffers()
{
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    // iterate through the image views and create a framebuffer for each of them
    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
         // must be consitent with createRenderPass()
        std::vector<VkImageView> attachments;
        if (m_useColorAttachmentResolve)
            attachments.push_back(m_colorImage.getImageView());
        else
            attachments.push_back(m_swapChainImageViews.at(i));

        if(m_useDepthBuffer)
            attachments.push_back(m_depthImage.getImageView());

        if (m_useColorAttachmentResolve)
            attachments.push_back(m_swapChainImageViews.at(i));

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_graphicsPipeline_mesh.getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_contextPtr->getDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers.at(i)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    infoLog() << "createFramebuffers(): OK ";
}


/*
 * Setup depth-buffer
 */
void DemoApp::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    m_depthImage.createImage(*m_contextPtr,
                             m_swapChainExtent.width, m_swapChainExtent.height, m_msaaSamples,
                             depthFormat,
                             VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    m_depthImage.createImageView(*m_contextPtr, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    m_depthImage.transitionImageLayout(*m_contextPtr, depthFormat,
                                       VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}


/*
 * Sorts a given list of candidate formats from most desirable to least desirable, 
 * and checks which is the first one that is supported
 */
VkFormat DemoApp::findSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features)
{
    for (VkFormat format : _candidates) 
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_contextPtr->getPhysicalDevice(), format, &props);

        if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features) {
            return format;
        }
        else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}


/*
 * Helper function to select a format with a depth component that supports usage as depth attachment
 */
VkFormat DemoApp::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}


/*
 * Creates a multisampled color buffer
 */
void DemoApp::createColorResources()
{
    VkFormat colorFormat = m_swapChainImageFormat;

    m_colorImage.createImage(*m_contextPtr,
                             m_swapChainExtent.width, m_swapChainExtent.height, m_msaaSamples,
                             colorFormat,
                             VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    m_colorImage.createImageView(*m_contextPtr, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}


/*
 * Creation of Uniforms buffer
 */
void DemoApp::createUniformBuffers() 
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        createBuffer(m_contextPtr->getPhysicalDevice(), m_contextPtr->getDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     m_uniformBuffers.at(i), m_uniformBuffersMemory.at(i));

        vkMapMemory(m_contextPtr->getDevice(), m_uniformBuffersMemory.at(i), 0, bufferSize, 0, &m_uniformBuffersMapped.at(i));
    }
}


/*
 * Descriptors allocation from a pool
 */
void DemoApp::createDescriptorPools() 
{
    // 1. Graphics descriptor pool
    m_graphicsPipeline_mesh.createComputeDescriptorPools(m_contextPtr->getDevice(), static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
    m_graphicsPipeline_particles.createComputeDescriptorPools(m_contextPtr->getDevice(), static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

    // 2. Compute descriptor pool
    m_computePipeline.createComputeDescriptorPools(m_contextPtr->getDevice(), static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));


    infoLog() << "createDescriptorPools(): OK ";
}


/*
 * Allocates the descriptor sets for graphics and compute pipelines
 */
void DemoApp::createDescriptorSets()
{
    // 1. Descriptor sets for graphics pipeline
    m_graphicsPipeline_mesh.createGraphicsDescriptorSet(m_contextPtr->getDevice(), MAX_FRAMES_IN_FLIGHT, m_uniformBuffers, m_textureImage);
    m_graphicsPipeline_particles.createGraphicsDescriptorSet(m_contextPtr->getDevice(), MAX_FRAMES_IN_FLIGHT, m_uniformBuffers, m_textureImage);


    // 2. Descriptor sets for compute pipeline
    m_computePipeline.createComputeDescriptorSet(m_contextPtr->getDevice(), MAX_FRAMES_IN_FLIGHT, 
                                                 m_uniformBuffers, m_computeShaderStorageBuffers,
                                                 PARTICLE_COUNT);
}


/*
 * Creation of command buffers
 */
void DemoApp::createCommandBuffers()
{
    //1. Graphics pipeline Command Buffer for mesh
    m_graphicsPipeline_mesh.createCommandBuffers(m_contextPtr->getDevice(), m_contextPtr->getCommandPool(), MAX_FRAMES_IN_FLIGHT);

    // 1.5. Graphics pipeline Command Buffer particles
    m_graphicsPipeline_particles.createCommandBuffers(m_contextPtr->getDevice(), m_contextPtr->getCommandPool(), MAX_FRAMES_IN_FLIGHT);
   
    // 2. Compute pipeline Command Buffer particles
    m_computePipeline.createCommandBuffers(m_contextPtr->getDevice(), m_contextPtr->getCommandPool(), MAX_FRAMES_IN_FLIGHT);

    infoLog() << "createCommandBuffers(): OK ";
}


/*
 * Writes commands for graphics shader into a command buffer
 */
void DemoApp::recordGraphicsCommandBuffer(Pipeline _graphicsPipeline, 
                                          VkBuffer _vertexBuffer, VkBuffer _indexBuffer, 
                                          uint32_t _vertexCount, uint32_t _imageIndex)
{
    if (_vertexBuffer == nullptr)
    {
        errorLog() << "DemoApp::recordGraphicsCommandBuffer(): _vertexBuffer is null ! "; 
        return;
    }

    VkCommandBuffer commandBuffer = _graphicsPipeline.getCommandBuffers().at(m_currentFrame);
   
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }


    // Prepares render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _graphicsPipeline.getRenderPass();
    renderPassInfo.framebuffer = m_swapChainFramebuffers.at(_imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues.at(0).color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues.at(1).depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    // Begins render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    {
        // Basic drawing commands
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.getPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapChainExtent.width);
        viewport.height = static_cast<float>(m_swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


        // Bind vertex buffer
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertexBuffer, offsets);
 
        // Bind index buffer
        if (_indexBuffer != nullptr)
        {
            vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32 /*VK_INDEX_TYPE_UINT16*/);
        }

        // Bind descriptors (i.e., uniforms)
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.getPipelineLayout(), 0, 1, &_graphicsPipeline.getDescriptorSets().at(m_currentFrame), 0, nullptr);

        // Issue draw command !
        if (_indexBuffer != nullptr)
        {
            vkCmdDrawIndexed(commandBuffer, _vertexCount, 1, 0, 0, 0); // indexed vertex buffer version
        }
        else
        {
            vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0); // unindexed vertex buffer version
        }

    }

    // Ends render pass
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}



/*
 * Writes commands for compute shader into a command buffer
 */
void DemoApp::recordComputeCommandBuffer(VkCommandBuffer _commandBuffer)
{
    vkResetCommandBuffer(_commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.getPipeline());

    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.getPipelineLayout(), 0, 1, &m_computePipeline.getDescriptorSets().at(m_currentFrame), 0, nullptr);

    vkCmdDispatch(_commandBuffer, PARTICLE_COUNT / 256, 1, 1);

    if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
}


/*
 * Creation of semaphores and fences
 */
void DemoApp::createSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_contextPtr->getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateSemaphore(m_contextPtr->getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateFence(m_contextPtr->getDevice(), &fenceInfo, nullptr, &m_inFlightFences.at(i)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
        if (vkCreateSemaphore(m_contextPtr->getDevice(), &semaphoreInfo, nullptr, &m_computeFinishedSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateFence(m_contextPtr->getDevice(), &fenceInfo, nullptr, &m_computeInFlightFences.at(i)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }
    }

    infoLog() << "createSyncObjects(): OK ";
}


/*
 * Drawing function
 */


void DemoApp::drawFrame()
{
    if (DRAW_PARTICLES || DRAW_MESH)
    {

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // 1. Compute submission        
        vkWaitForFences(m_contextPtr->getDevice(), 1, &m_computeInFlightFences.at(m_currentFrame), VK_TRUE, UINT64_MAX);

        vkResetFences(m_contextPtr->getDevice(), 1, &m_computeInFlightFences.at(m_currentFrame));

        recordComputeCommandBuffer(m_computePipeline.getCommandBuffers().at(m_currentFrame));

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_computePipeline.getCommandBuffers().at(m_currentFrame);
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores.at(m_currentFrame);

        if (vkQueueSubmit(m_contextPtr->getComputeQueue(), 1, &submitInfo, m_computeInFlightFences.at(m_currentFrame)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit compute command buffer!");
        };


        // 2. Graphics submission Particles and Mesh
        vkWaitForFences(m_contextPtr->getDevice(), 1, &m_inFlightFences.at(m_currentFrame), VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_contextPtr->getDevice(), m_swapChain, UINT64_MAX,
        m_imageAvailableSemaphores.at(m_currentFrame), VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(m_currentFrame);

        // Only reset the fence if we are submitting work
        vkResetFences(m_contextPtr->getDevice(), 1, &m_inFlightFences.at(m_currentFrame));

        vkResetCommandBuffer(m_graphicsPipeline_particles.getCommandBuffers().at(m_currentFrame), 0);
        recordGraphicsCommandBuffer(m_graphicsPipeline_particles, 
                                    m_computeShaderStorageBuffers.at(m_currentFrame), nullptr, 
                                    PARTICLE_COUNT, imageIndex);
        vkResetCommandBuffer(m_graphicsPipeline_mesh.getCommandBuffers().at(m_currentFrame), 0);
        recordGraphicsCommandBuffer(m_graphicsPipeline_mesh, 
                                    m_mesh.getVertexBuffer(), m_mesh.getIndexBuffer(), 
                                    static_cast<uint32_t>(m_mesh.getIndices().size()), imageIndex);
        std::array<VkSemaphore, 2> waitSemaphores = { m_computeFinishedSemaphores.at(m_currentFrame), m_imageAvailableSemaphores.at(m_currentFrame) };
        std::array<VkPipelineStageFlags, 2> waitStages = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        std::vector<VkCommandBuffer> cmdBuffers;
        if (DRAW_PARTICLES)
        {
            cmdBuffers.push_back(m_graphicsPipeline_particles.getCommandBuffers().at(m_currentFrame));
        }
        if (DRAW_MESH)
        {
            cmdBuffers.push_back(m_graphicsPipeline_mesh.getCommandBuffers().at(m_currentFrame));
        }

        submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());
        submitInfo.pCommandBuffers = cmdBuffers.data();

        std::array<VkSemaphore, 1> signalSemaphores = { m_renderFinishedSemaphores.at(m_currentFrame) };
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        if (vkQueueSubmit(m_contextPtr->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences.at(m_currentFrame)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores.data();
        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(m_contextPtr->getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
        {
            m_framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    // update current frame id
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // We want to animate the particle system using the last frames time to get smooth, frame-rate independent animation
    double currentTime = glfwGetTime();
    m_lastFrameTime = (currentTime - m_lastTime) * 1000.0;
    m_lastTime = currentTime; 
}


/*
 * Cleanup the swap chain before recreating it
 */
void DemoApp::cleanupSwapChain() 
{
    m_colorImage.cleanup(*m_contextPtr);
    m_depthImage.cleanup(*m_contextPtr);

    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_contextPtr->getDevice(), m_swapChainFramebuffers.at(i), nullptr);
    }

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        vkDestroyImageView(m_contextPtr->getDevice(), m_swapChainImageViews.at(i), nullptr);
    }

    vkDestroySwapchainKHR(m_contextPtr->getDevice(), m_swapChain, nullptr);
}


/*
 * Recreate the swap chain whenever event happens
 */
void DemoApp::recreateSwapChain() 
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_contextPtr->getDevice());

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createColorResources();
    createDepthResources();
    createFramebuffers();
}


/*
 * Generates a new transformation every frame to make the geometry spin around
 */
void DemoApp::updateUniformBuffer(uint32_t _currentImage) 
{
    //static auto startTime = std::chrono::high_resolution_clock::now();
    //auto currentTime = std::chrono::high_resolution_clock::now();
    //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    //m_initModel = glm::rotate(m_initModel, glm::radians(0.05f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_ubo.model = m_trackball.getRotationMatrix() 
                * m_initModel;

    m_ubo.deltaTime = static_cast<float>(m_lastFrameTime) * 2.0f;  

    float random_offset = rndDist(rndEngine) * 0.5f;
    m_ubo.windX = static_cast<float>(cos( m_lastTime + random_offset )) * 0.00002f;

    memcpy(m_uniformBuffersMapped.at(_currentImage), &m_ubo, sizeof(m_ubo));
}


/*
 * Window resize callback
 */
void DemoApp::framebufferResizeCallback(GLFWwindow* _window, int _width, int _height)
{
    auto app = reinterpret_cast<DemoApp*>(glfwGetWindowUserPointer(_window));
    app->m_framebufferResized = true;
}

/*
 * Keyboard event callback
 */
void DemoApp::keyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    // return to init positon when "R" pressed
    if (_key == GLFW_KEY_R && _action == GLFW_PRESS)
    {
        auto app = reinterpret_cast<DemoApp*>(glfwGetWindowUserPointer(_window));
        app->m_trackball.reStart();
    }
}

/*
 * Mouse button event callback
 */
void DemoApp::mouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
    auto app = reinterpret_cast<DemoApp*>(glfwGetWindowUserPointer(_window));

    // get mouse cursor position
    double x, y;
    glfwGetCursorPos(_window, &x, &y);

    // activate/de-activate trackball with mouse button
    if (_action == GLFW_PRESS) 
    {
        if (_button == GLFW_MOUSE_BUTTON_LEFT)
            app->m_trackball.startTracking( glm::vec2(x, y) );
    }
    else 
    {
        if (_button == GLFW_MOUSE_BUTTON_LEFT)
            app->m_trackball.stopTracking();
    }
    
}

/*
 * Mouse scroll event callback
 */
void DemoApp::scrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
}


/*
 * Mouse cursor event callback
 */
void DemoApp::cursorPosCallback(GLFWwindow* _window, double _x, double _y)
{
    auto app = reinterpret_cast<DemoApp*>(glfwGetWindowUserPointer(_window));

    // rotate trackball according to mouse cursor movement
    if ( app->m_trackball.isTracking()) 
        app->m_trackball.move( glm::vec2(_x, _y) );
}


/*
 * Fetch max nb of samples
 */
VkSampleCountFlagBits DemoApp::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_contextPtr->getPhysicalDevice(), &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}


void DemoApp::createComputeShaderStorageBuffers() 
{
    // Initialize particles

    glm::vec4 originProj = m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0) ;
    float depthOrigin = originProj.z/originProj.w;
    float depthOffest = 1.0f - depthOrigin;

    std::vector<Particle> particles(PARTICLE_COUNT);
    initParticles(particles, depthOrigin);

    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

    // Create a staging buffer used to upload data to the gpu
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(m_contextPtr->getPhysicalDevice(), m_contextPtr->getDevice(), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_contextPtr->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, particles.data(), (size_t)bufferSize);
    vkUnmapMemory(m_contextPtr->getDevice(), stagingBufferMemory);

    m_computeShaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_computeShaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    // Copy initial particle data to all storage buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        createBuffer(m_contextPtr->getPhysicalDevice(), m_contextPtr->getDevice(), bufferSize, 
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                     m_computeShaderStorageBuffers.at(i), m_computeShaderStorageBuffersMemory.at(i));
        copyBuffer(m_contextPtr->getDevice(), m_contextPtr->getCommandPool(), m_contextPtr->getGraphicsQueue(), 
                   stagingBuffer, m_computeShaderStorageBuffers.at(i), bufferSize);
    }

    vkDestroyBuffer(m_contextPtr->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_contextPtr->getDevice(), stagingBufferMemory, nullptr);

}


} // namespace VulkanDemo