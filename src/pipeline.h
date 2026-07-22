/*********************************************************************************************************************
 *
 * pipeline.h
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#ifndef PIPELINE_H
#define PIPELINE_H

#include "utils.h"


namespace VulkanDemo
{
    class Image;

class Pipeline
{
    

public:

    Pipeline() = default;

    Pipeline(Pipeline const& _other) = default;

    Pipeline& operator=(Pipeline const& _other)
    {
        m_descriptorSetLayout = _other.m_descriptorSetLayout;
        m_descriptorPool = _other.m_descriptorPool;
        m_descriptorSets = _other.m_descriptorSets;
        m_renderPass = _other.m_renderPass;
        m_pipelineLayout = _other.m_pipelineLayout;
        m_pipeline = _other.m_pipeline;
        m_commandBuffers = _other.m_commandBuffers;
        return *this;
    }

    Pipeline(Pipeline&& _other)
        : m_descriptorSetLayout(_other.m_descriptorSetLayout)
        , m_descriptorPool(_other.m_descriptorPool)
        , m_descriptorSets(std::move(_other.m_descriptorSets))
        , m_renderPass(_other.m_renderPass)
        , m_pipelineLayout(_other.m_pipelineLayout)
        , m_pipeline(_other.m_pipeline)
        , m_commandBuffers(std::move(_other.m_commandBuffers))
     {}

    Pipeline& operator=(Pipeline&& _other)
    {
        m_descriptorSetLayout = _other.m_descriptorSetLayout;
        m_descriptorPool = _other.m_descriptorPool;
        m_descriptorSets =  std::move(_other.m_descriptorSets);
        m_renderPass = _other.m_renderPass;
        m_pipelineLayout = _other.m_pipelineLayout;
        m_pipeline = _other.m_pipeline;
        m_commandBuffers = std::move(_other.m_commandBuffers);
        return *this;
    }

    virtual ~Pipeline() {};


    VkDescriptorSetLayout const& getDescriptorSetLayout() const { return m_descriptorSetLayout; }
    VkDescriptorPool const& getDescriptorPool() const { return m_descriptorPool; }
    std::vector<VkDescriptorSet> const& getDescriptorSets() const { return m_descriptorSets; }
    VkRenderPass const& getRenderPass() const { return m_renderPass; }
    VkPipelineLayout const& getPipelineLayout() const { return m_pipelineLayout; }
    VkPipeline const& getPipeline() const { return m_pipeline; }
    std::vector<VkCommandBuffer> const& getCommandBuffers() const { return m_commandBuffers; }


    void createGraphicsDescriptorSetLayout(VkDevice _device);
    void createComputeDescriptorSetLayout(VkDevice _device);

    void createGraphicsDescriptorPools(VkDevice _device, const uint32_t _descriptorCount);
    void createComputeDescriptorPools(VkDevice _device, const uint32_t _descriptorCount);

    void createGraphicsDescriptorSet(VkDevice _device, const uint32_t _descriptorCount, 
                                     const std::vector<VkBuffer>& _uniformBuffers, 
                                     Image& _textureImage);
    void createComputeDescriptorSet(VkDevice _device, const uint32_t _descriptorCount, 
                                     const std::vector<VkBuffer>& _uniformBuffers, 
                                     const std::vector<VkBuffer>& _computeShaderStorageBuffers,
                                     const int& _nbParticles);

    void createRenderPass(VkDevice _device, VkFormat _swapChainImageFormat,
                          bool _useDepthBuffer, bool _useColorAttachmentResolve, bool _clearBuffers,
                          VkSampleCountFlagBits _sampleCount, VkFormat _depthFormat);

    //VK_POLYGON_MODE_FILL = 0,
    //VK_POLYGON_MODE_LINE = 1,
    //VK_POLYGON_MODE_POINT = 2,
    //see https://docs.vulkan.org/refpages/latest/refpages/source/VkPolygonMode.html
    //
    //VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    //VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    //see https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkPrimitiveTopology.html

    //VK_CULL_MODE_NONE = 0,
    //VK_CULL_MODE_FRONT_BIT = 1,
    //VK_CULL_MODE_BACK_BIT = 2,
    //VK_CULL_MODE_FRONT_AND_BACK = 3,
    //see https://docs.vulkan.org/refpages/latest/refpages/source/VkCullModeFlagBits.html
    void createGraphicsPipeline(VkDevice _device,
                                VkShaderModule _vertShaderModule, VkShaderModule _fragShaderModule,
                                VkPipelineVertexInputStateCreateInfo _vertexInputInfo,
                                bool _useDepthBuffer, VkSampleCountFlagBits _sampleCount,
                                unsigned int _polygoneMode, unsigned int _topology, unsigned int _cullMode);

    void createComputePipeline( VkDevice _device,
                                VkShaderModule _compShaderModule);

    void createCommandBuffers(VkDevice _device, VkCommandPool _commandPool, const int _inFlightFrames);

    void destroy(VkDevice _device, VkCommandPool _commandPool);


protected:

    // Descriptors (i.e., uniforms)
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;    // descriptors layout
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;              // descriptor pool
    std::vector<VkDescriptorSet>  m_descriptorSets; // descriptors (for each in-flight frame)

    VkRenderPass m_renderPass = VK_NULL_HANDLE;          // the render pipeline
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;  // defines uniforms
    VkPipeline m_pipeline = VK_NULL_HANDLE;              // final pipeline

    std::vector<VkCommandBuffer> m_commandBuffers;       // Command buffer (for each in-flight frame)


}; // class Pipeline

} // namespace VulkanDemo


#endif // PIPELINE_H