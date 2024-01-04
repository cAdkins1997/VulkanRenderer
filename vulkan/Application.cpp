
#include "Application.h"

renderer::Application::Application() {
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
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

void renderer::Application::createPipeline(){

    assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
            device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            pipelineConfig);
}

void renderer::Application::createCommandBuffers() {
    commandBuffers.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo commandBufferAI;
    commandBufferAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAI.commandPool = device.getCommandPool();
    commandBufferAI.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    commandBufferAI.pNext = nullptr;

    if (vkAllocateCommandBuffers(device.device(), &commandBufferAI, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!\n");
    }
}

void renderer::Application::drawFrame() {
    uint32_t imageIndex;
    auto result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!\n");
    }

    recordCommandBuffers(imageIndex);
    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }

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

void renderer::Application::recreateSwapChain() {
    auto extent = window.getExtent();

    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        glfwWaitEvents();
    }

    swapChain = nullptr;
    vkDeviceWaitIdle(device.device());

    if (swapChain == nullptr) {
        swapChain = std::make_unique<SwapChain>(device, extent);
    }
    else  {
        swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));

        if (swapChain->imageCount() != commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
    }

    createPipeline();
}

void renderer::Application::recordCommandBuffers(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer\n");
    }

    VkRenderPassBeginInfo renderPassBI{};
    renderPassBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBI.renderPass = swapChain->getRenderPass();
    renderPassBI.framebuffer = swapChain->getFrameBuffer(imageIndex);

    renderPassBI.renderArea.offset = {0, 0};
    renderPassBI.renderArea.extent = { swapChain->getSwapChainExtent() };

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0 };
    renderPassBI.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBI.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBI, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    pipeline->bind(commandBuffers[imageIndex]);
    model->bind(commandBuffers[imageIndex]);
    model->draw(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffers\n");
    }
}

void renderer::Application::freeCommandBuffers() {
    vkFreeCommandBuffers(device.device(),
                         device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
}
