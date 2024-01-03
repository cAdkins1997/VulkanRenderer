
#ifndef VULKANLEARN_APPLICATION_H
#define VULKANLEARN_APPLICATION_H

#include "Pipeline.h"
#include "Window.h"
#include "Device.hpp"
#include "SwapChain.hpp"

#include <memory>
#include <stdexcept>
#include <array>

namespace renderer {
    class Application {
    public:
        static constexpr int WIDTH = 1920, HEIGHT = 1080;

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator = (const Application&) = delete;
        void run();

    private:

        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        Window window{WIDTH, HEIGHT, "Vulkan"};
        Device device{window};
        SwapChain swapChain{device, window.getExtent()};
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
    };
}


#endif //VULKANLEARN_APPLICATION_H
