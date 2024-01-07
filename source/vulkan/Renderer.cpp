
#include "Renderer.h"


namespace rendering {

    rendering::Renderer::Renderer(Window& _window, Device& _device)
    : window(_window), device(_device){
        recreateSwapChain();
        createCommandBuffers();
    }

    rendering::Renderer::~Renderer() {
        freeCommandBuffers();
    }

    void rendering::Renderer::createCommandBuffers() {
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

    void rendering::Renderer::recreateSwapChain() {
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
    }

    void rendering::Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(),
                             device.getCommandPool(),
                             static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        assert(!isFrameStarted && "Cannot call begin frame while a frame is already in progress");

        auto result = swapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!\n");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer\n");
        }

        return commandBuffer;
    }

    void Renderer::endFrame() {
        assert(isFrameStarted && "can't call end frame while a frame is not in progress");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffers\n");
        }

        auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
            window.resetWindowResizedFlag();
            recreateSwapChain();
        }

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to submit command buffers\n");
        }

        isFrameStarted = false;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "can't begin swap chain render pass while a frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render on a command buffer from a different frame");

        VkRenderPassBeginInfo renderPassBI{};
        renderPassBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBI.renderPass = swapChain->getRenderPass();
        renderPassBI.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

        renderPassBI.renderArea.offset = {0, 0};
        renderPassBI.renderArea.extent = { swapChain->getSwapChainExtent() };

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0 };
        renderPassBI.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBI.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBI, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "can't end swap chain render pass while a frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't end render on a command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
};