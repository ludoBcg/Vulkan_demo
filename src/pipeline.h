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

class Pipeline
{
    

public:

    Pipeline() = default;

    Pipeline(Pipeline const& _other) = default;

    Pipeline& operator=(Pipeline const& _other)
    {
        m_renderPass = _other.m_renderPass;
        m_pipelineLayout = _other.m_pipelineLayout;
        m_pipeline = _other.m_pipeline;
        return *this;
    }

    Pipeline(Pipeline&& _other)
        : m_renderPass(_other.m_renderPass)
        , m_pipelineLayout(_other.m_pipelineLayout)
        , m_pipeline(_other.m_pipeline)
     {}

    Pipeline& operator=(Pipeline&& _other)
    {
        m_renderPass = _other.m_renderPass;
        m_pipelineLayout = _other.m_pipelineLayout;
        m_pipeline = _other.m_pipeline;
        return *this;
    }

    virtual ~Pipeline() {};


    VkRenderPass const& getRenderPass() const { return m_renderPass; }
    VkPipelineLayout const& getPipelineLayout() const { return m_pipelineLayout; }
    VkPipeline const& getPipeline() const { return m_pipeline; }

    void createRenderPass(VkDevice _device, VkFormat _swapChainImageFormat,
                          bool _useDepthBuffer, bool _useColorAttachmentResolve, 
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
                                VkVertexInputBindingDescription _bindingDescription,
                                VkDescriptorSetLayout _descriptorSetLayout,
                                std::vector<VkVertexInputAttributeDescription>& _attributeDescriptions,
                                bool _useDepthBuffer, VkSampleCountFlagBits _sampleCount, 
                                unsigned int _polygonMode,  unsigned int _topology, unsigned int _cullMode);

    void createComputePipeline( VkDevice _device,
                                VkShaderModule _compShaderModule,
                                VkDescriptorSetLayout _descriptorSetLayout);

    void destroy(VkDevice _device);


protected:

    VkRenderPass m_renderPass = VK_NULL_HANDLE;          // the render pipeline
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;  // defines uniforms
    VkPipeline m_pipeline = VK_NULL_HANDLE;              // final pipeline


}; // class Pipeline

} // namespace VulkanDemo


#endif // PIPELINE_H