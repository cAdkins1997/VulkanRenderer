
#include "RenderSystem.h"

struct pushConstantsData {
    glm::mat4 transform{1.0f};
    glm::mat4 normalMatrix{1.0f};
};

rendering::RenderSystem::RenderSystem(rendering::Device& _device, VkRenderPass renderPass) : device(_device)  {
    createPipelineLayout();
    createPipeline(renderPass);
}

rendering::RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void rendering::RenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<engine::Object> &objects, const Camera& camera) {
    pipeline->bind(commandBuffer);

    auto projectionView = camera.getProjection() * camera.getView();

    for (auto& object : objects) {
        pushConstantsData push{};
        auto modelMatrix = object.transform.mat4();
        push.transform = projectionView * modelMatrix;
        push.normalMatrix = object.transform.normalMatrix();

        vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(pushConstantsData),
                &push);

        object.model->bind(commandBuffer);
        object.model->draw(commandBuffer);
    }
}

void rendering::RenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(pushConstantsData);

    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 0;
    pipelineLayoutCI.pSetLayouts = nullptr;
    pipelineLayoutCI.pushConstantRangeCount = 1;
    pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout\n");
    }
}

void rendering::RenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
            device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            pipelineConfig);
}
