
#ifndef VULKANLEARN_RENDERER_H
#define VULKANLEARN_RENDERER_H

#include "Window.h"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Model.h"

#include <memory>
#include <stdexcept>
#include <array>
#include <cassert>

#include <glm/gtc/constants.hpp>

namespace rendering {
    class Renderer {
    public:
        Renderer(Window& _window, Device& _device);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer &operator = (const Renderer&) = delete;

        [[nodiscard]] VkCommandBuffer beginFrame();
        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
        [[nodiscard]] bool isFrameInProgress() const { return isFrameStarted; }
        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentImageIndex];
        }


    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window& window;
        Device& device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        bool isFrameStarted;
    };
}


#endif //VULKANLEARN_RENDERER_H
