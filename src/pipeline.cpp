/*********************************************************************************************************************
 *
 * pipeline.cpp
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#include "pipeline.h"


namespace VulkanDemo
{


/*
 * Creation of descriptor set layout for a graphics pipeline
 */
void Pipeline::createGraphicsDescriptorSetLayout(VkDevice _device)
{
    // UniformBufferObject binding (cf. vertex shader layout(binding = 0) uniform)
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    // sampler2D (i.e., texture) binding (cf. fragment shader layout(binding = 1) uniform)
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> graphicsBindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo graphicsLayoutInfo{};
    graphicsLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    graphicsLayoutInfo.bindingCount = static_cast<uint32_t>(graphicsBindings.size());
    graphicsLayoutInfo.pBindings = graphicsBindings.data();

    if (vkCreateDescriptorSetLayout(_device, &graphicsLayoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}


/*
 * Creation of descriptor set layout for a compute pipeline
 */
void Pipeline::createComputeDescriptorSetLayout(VkDevice _device)
{
    // UniformBufferObject binding (cf. compute shader layout(binding = 0) uniform)
    std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
    layoutBindings.at(0).binding = 0;
    layoutBindings.at(0).descriptorCount = 1;
    layoutBindings.at(0).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings.at(0).pImmutableSamplers = nullptr;
    layoutBindings.at(0).stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    // Shader Storage Buffer Object (SSBO) input binding (cf. compute shader layout(binding = 1) readonly buffer)
    layoutBindings.at(1).binding = 1;
    layoutBindings.at(1).descriptorCount = 1;
    layoutBindings.at(1).descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings.at(1).pImmutableSamplers = nullptr;
    layoutBindings.at(1).stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    //  Shader Storage Buffer Object (SSBO) output binding (cf. compute shader layout(binding = 2) buffer)
    layoutBindings.at(2).binding = 2;
    layoutBindings.at(2).descriptorCount = 1;
    layoutBindings.at(2).descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings.at(2).pImmutableSamplers = nullptr;
    layoutBindings.at(2).stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo computeLayoutInfo{};
    computeLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    computeLayoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    computeLayoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(_device, &computeLayoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }

    infoLog() << "createComputeDescriptorSetLayout(): OK ";
}


/*
 * Descriptors allocation from a pool
 */
void Pipeline::createGraphicsDescriptorPools(VkDevice _device, const uint32_t _descriptorCount) 
{
    // Two descriptors: uniforms and sampler
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes.at(0).type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes.at(0).descriptorCount = _descriptorCount;
    poolSizes.at(1).type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes.at(1).descriptorCount = _descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = _descriptorCount;

    if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

}


/*
 * Descriptors allocation from a pool
 */
void Pipeline::createComputeDescriptorPools(VkDevice _device, const uint32_t _descriptorCount) 
{
    std::vector<VkDescriptorPoolSize> poolSizes(1);
    poolSizes.at(0).type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes.at(0).descriptorCount = _descriptorCount;
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = _descriptorCount;

    if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}




/*
 * Creation of render pass 
 */
void Pipeline::createRenderPass(VkDevice _device, VkFormat _swapChainImageFormat,
                                bool _useDepthBuffer, bool _useColorAttachmentResolve,
                                VkSampleCountFlagBits _sampleCount, VkFormat _depthFormat)
{
    // defines color attachment (attachment 0)
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = _sampleCount;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = _useColorAttachmentResolve ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // subpass will reference color attachment
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    // defines depth attachment (attachment 1)
    VkAttachmentDescription depthAttachment{};
    VkAttachmentReference depthAttachmentRef{};
    if (_useDepthBuffer)
    {
        depthAttachment.format = _depthFormat;
        depthAttachment.samples = _sampleCount;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        // subpass will reference depth attachment
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // define resolve attachment for multisampling (attachment 3)
    VkAttachmentDescription colorAttachmentResolve{};
    VkAttachmentReference colorAttachmentResolveRef{};
    if (_useColorAttachmentResolve)
    {
        colorAttachmentResolve.format = _swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // subpass will reference resolve attachment
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // defines a subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = ( _useDepthBuffer ? &depthAttachmentRef : nullptr );
    subpass.pResolveAttachments = ( _useColorAttachmentResolve ? &colorAttachmentResolveRef : nullptr );
    
    

    // Subpass dependencies
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    if (_useDepthBuffer) 
    {
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    else 
    {
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    // assemble info to build render pass
    std::vector<VkAttachmentDescription> attachments = { colorAttachment };
    if (_useDepthBuffer) { attachments.push_back(depthAttachment); }
    if (_useColorAttachmentResolve) { attachments.push_back(colorAttachmentResolve); }

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;


    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    infoLog() << "createRenderPass(): OK ";
}


void Pipeline::createGraphicsPipeline(VkDevice _device,
                                      VkShaderModule _vertShaderModule, VkShaderModule _fragShaderModule,
                                      VkPipelineVertexInputStateCreateInfo _vertexInputInfo,
                                     /* VkDescriptorSetLayout _descriptorSetLayout,*/
                                      bool _useDepthBuffer, VkSampleCountFlagBits _sampleCount,
                                      unsigned int _polygoneMode, unsigned int _topology, unsigned int _cullMode)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = _vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = _fragShaderModule;
    fragShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };


    // Describes what kind of geometry will be drawn from the vertices and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch (_topology)
    {
        case 0: inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case 3: inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        default: errorLog() << "Pipeline::createGraphicsPipeline(): topology must be either 0 (point list), or 3 (triangle list) "; 
                 inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
    }
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    // defines rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    switch (_polygoneMode)
    {
        case 0: rasterizer.polygonMode = VK_POLYGON_MODE_FILL; break;
        case 1: rasterizer.polygonMode = VK_POLYGON_MODE_LINE; break;
        case 2: rasterizer.polygonMode = VK_POLYGON_MODE_POINT; break;
        default: errorLog() << "Pipeline::createGraphicsPipeline(): polygonMode must be either 0 (fill), 1 (wireframe), or 2 (points) "; 
                 rasterizer.polygonMode = VK_POLYGON_MODE_FILL; break;
    }
    rasterizer.lineWidth = 1.0f;
    switch (_cullMode)
    {
        case 0: rasterizer.cullMode = VK_CULL_MODE_NONE; break;
        case 1: rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; break;
        case 2: rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case 3: rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
        default: errorLog() << "Pipeline::createGraphicsPipeline(): cullMode must be either 0 (none), 1 (front), 2 (back), or 3 (front and back) "; 
                 rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; break;
    }
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // configures multisampling for anti-aliasing
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE; // use VK_TRUE to enable sample shading in the pipeline
    multisampling.rasterizationSamples = _sampleCount;
    multisampling.minSampleShading = 1.0f; // use .2f as min fraction for sample shading, closer to one is smoother
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // depth buffer
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    if (_useDepthBuffer)
    {
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional
    }

    // Defines blending between fragment shader output color with the color that is already in the framebuffer
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //VK_BLEND_FACTOR_ONE; //@@@
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    // limited amount of the state that can actually be changed without recreating the pipeline at draw time
    // we enables dynamic viewport and scissor
    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = ( m_descriptorSetLayout == VK_NULL_HANDLE ? 0 : 1 );
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }


    // Assemble info for creation of graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = ( _useDepthBuffer ? &depthStencil : nullptr );
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout; // references the structures describing the fixed-function stage
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    // Finally creates the pipeline
    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    

    vkDestroyShaderModule(_device, _fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, _vertShaderModule, nullptr);
    

    infoLog() << "createGraphicsPipeline(): OK ";
}


/*
 * Creation of compute shader pipeline
 */
void Pipeline::createComputePipeline( VkDevice _device,
                                      VkShaderModule _compShaderModule/*,
                                      VkDescriptorSetLayout _descriptorSetLayout*/ )
{
    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = _compShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(_device, _compShaderModule, nullptr);
    infoLog() << "createComputePipeline(): OK ";
}


void Pipeline::createCommandBuffers(VkDevice _device, VkCommandPool _commandPool, const int _inFlightFrames)
{
    m_commandBuffers.resize(_inFlightFrames);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}


void Pipeline::destroy(VkDevice _device, VkCommandPool _commandPool)
{
    if(m_descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(_device, m_descriptorPool, nullptr);
    if(m_descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(_device, m_descriptorSetLayout, nullptr);


    if(m_pipeline != VK_NULL_HANDLE)
	    vkDestroyPipeline(_device, m_pipeline, nullptr);
    if(m_pipelineLayout != VK_NULL_HANDLE)
	    vkDestroyPipelineLayout(_device, m_pipelineLayout, nullptr);
    if(m_renderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(_device, m_renderPass, nullptr);

    vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
    m_commandBuffers.clear();
}

} // namespace VulkanDemo