
#include "Pipeline.h"

renderer::Pipeline::Pipeline(renderer::Device& _device,
                             const std::string& _vertexShader,
                             const std::string& _fragmentShader,
                             const renderer::PipelineConfigInfo& _configInfo) : device{_device} {
    createGraphicsPipeline(_vertexShader, _fragmentShader, _configInfo);
}

std::vector<char> renderer::Pipeline::readFile(const std::string &filepath) {
    std::ifstream  file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filepath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void renderer::Pipeline::createGraphicsPipeline(const std::string &_vertexShader,
                                                const std::string &_fragmentShader,
                                                const renderer::PipelineConfigInfo &configInfo) {
    auto vertexCode = readFile(_vertexShader);
    auto fragCode = readFile(_fragmentShader);

    assert(configInfo.pipelineLayout != VK_NULL_HANDLE &&
    "Cannot create graphics pipeline:: no pipelineLayout provided in config info\n");

    assert(configInfo.renderPass != VK_NULL_HANDLE &&
           "Cannot create graphics pipeline:: no renderPass provided in config info\n");

    createShaderModule(vertexCode, &vertexShaderModule);
    createShaderModule(fragCode, &fragmentShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertexShaderModule;
    shaderStages[0].pName = "main";
    shaderStages[0].flags = 0;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragmentShaderModule;
    shaderStages[1].pName = "main";
    shaderStages[1].flags = 0;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;

    auto bindingDescriptions = renderer::Model::Vertex::getBindingDescription();
    auto attributeDescriptions = renderer::Model::Vertex::getAttributeDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputCI.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputCI.pVertexAttributeDescriptions = attributeDescriptions.data();
    vertexInputCI.pVertexBindingDescriptions = bindingDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCI.stageCount = 2;
    pipelineCI.pStages = shaderStages;
    pipelineCI.pVertexInputState = &vertexInputCI;
    pipelineCI.pInputAssemblyState = &configInfo.inputAssemblyCI;
    pipelineCI.pViewportState = &configInfo.viewportCI;
    pipelineCI.pRasterizationState = &configInfo.rasterizationCI;
    pipelineCI.pMultisampleState = &configInfo.multisampleCI;

    pipelineCI.pColorBlendState = &configInfo.colorBlendCI;
    pipelineCI.pDepthStencilState = &configInfo.depthStencilCI;
    pipelineCI.pDynamicState = &configInfo.dynamicStateCI;

    pipelineCI.layout = configInfo.pipelineLayout;
    pipelineCI.renderPass = configInfo.renderPass;
    pipelineCI.subpass = configInfo.subpass;

    pipelineCI.basePipelineIndex = -1;
    pipelineCI.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline\n");
    }
}

void renderer::Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
    VkShaderModuleCreateInfo shaderModuleCI{};
    shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCI.codeSize = code.size();
    shaderModuleCI.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device.device(), &shaderModuleCI, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module\n");
    }
}

void renderer::Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
    configInfo.inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyCI.primitiveRestartEnable = VK_FALSE;

    configInfo.viewportCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportCI.viewportCount = 1;
    configInfo.viewportCI.pViewports = nullptr;
    configInfo.viewportCI.scissorCount = 1;
    configInfo.viewportCI.pScissors = nullptr;

    configInfo.rasterizationCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationCI.depthClampEnable = VK_FALSE;
    configInfo.rasterizationCI.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationCI.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationCI.lineWidth = 1.0f;
    configInfo.rasterizationCI.cullMode = VK_CULL_MODE_NONE;
    configInfo.rasterizationCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizationCI.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationCI.depthBiasConstantFactor = 0.0f;  // Optional
    configInfo.rasterizationCI.depthBiasClamp = 0.0f;           // Optional
    configInfo.rasterizationCI.depthBiasSlopeFactor = 0.0f;     // Optional

    configInfo.multisampleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleCI.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleCI.minSampleShading = 1.0f;           // Optional
    configInfo.multisampleCI.pSampleMask = nullptr;             // Optional
    configInfo.multisampleCI.alphaToCoverageEnable = VK_FALSE;  // Optional
    configInfo.multisampleCI.alphaToOneEnable = VK_FALSE;       // Optional

    configInfo.colorBlendAttachmentCI.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachmentCI.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachmentCI.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachmentCI.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachmentCI.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    configInfo.colorBlendAttachmentCI.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachmentCI.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachmentCI.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    configInfo.colorBlendCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendCI.logicOpEnable = VK_FALSE;
    configInfo.colorBlendCI.logicOp = VK_LOGIC_OP_COPY;  // Optional
    configInfo.colorBlendCI.attachmentCount = 1;
    configInfo.colorBlendCI.pAttachments = &configInfo.colorBlendAttachmentCI;
    configInfo.colorBlendCI.blendConstants[0] = 0.0f;  // Optional
    configInfo.colorBlendCI.blendConstants[1] = 0.0f;  // Optional
    configInfo.colorBlendCI.blendConstants[2] = 0.0f;  // Optional
    configInfo.colorBlendCI.blendConstants[3] = 0.0f;  // Optional

    configInfo.depthStencilCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilCI.depthTestEnable = VK_TRUE;
    configInfo.depthStencilCI.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilCI.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilCI.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilCI.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencilCI.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencilCI.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilCI.front = {};  // Optional
    configInfo.depthStencilCI.back = {};   // Optional

    configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    configInfo.dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateCI.pDynamicStates = configInfo.dynamicStateEnables.data();
    configInfo.dynamicStateCI.dynamicStateCount = static_cast<uint32_t >(configInfo.dynamicStateEnables.size());
    configInfo.dynamicStateCI.flags = 0;
}

void renderer::Pipeline::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

renderer::Pipeline::~Pipeline() {
    vkDestroyShaderModule(device.device(), vertexShaderModule, nullptr);
    vkDestroyShaderModule(device.device(), fragmentShaderModule, nullptr);
    vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
}
