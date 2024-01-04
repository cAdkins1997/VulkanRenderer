
#include "Application.h"

renderer::Application::Application() {
    loadModels();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

renderer::Application::~Application() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void renderer::Application::run() {

    while(!window.shouldCLose()) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(device.device());
}

void renderer::Application::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 0;
    pipelineLayoutCI.pSetLayouts = nullptr;
    pipelineLayoutCI.pushConstantRangeCount = 0;
    pipelineLayoutCI.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layou\n");
    }
}

void renderer::Application::createPipeline() {
    auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain.width(), swapChain.height());
    pipelineConfig.renderPass = swapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
            device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            pipelineConfig);
}

void renderer::Application::createCommandBuffers() {
    commandBuffers.resize(swapChain.imageCount());

    VkCommandBufferAllocateInfo commandBufferAI;
    commandBufferAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAI.commandPool = device.getCommandPool();
    commandBufferAI.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    commandBufferAI.pNext = nullptr;

    if (vkAllocateCommandBuffers(device.device(), &commandBufferAI, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!\n");
    }

    for (int i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer\n");
        }

        VkRenderPassBeginInfo renderPassBI{};
        renderPassBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBI.renderPass = swapChain.getRenderPass();
        renderPassBI.framebuffer = swapChain.getFrameBuffer(i);

        renderPassBI.renderArea.offset = {0, 0};
        renderPassBI.renderArea.extent = { swapChain.getSwapChainExtent() };

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0 };
        renderPassBI.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBI.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBI, VK_SUBPASS_CONTENTS_INLINE);

        pipeline->bind(commandBuffers[i]);
        model->bind(commandBuffers[i]);
        model->draw(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffers\n");
        }
    }
}

void renderer::Application::drawFrame() {
    uint32_t imageIndex;
    auto result = swapChain.acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!\n");
    }

    result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffers\n");
    }
}

void renderer::Application::loadModels() {
    std::vector<Model::Vertex> vertices{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    model = std::make_unique<Model>(device, vertices);
}
