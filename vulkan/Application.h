
#ifndef VULKANLEARN_APPLICATION_H
#define VULKANLEARN_APPLICATION_H

#include "Pipeline.h"
#include "Window.h"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Model.h"

#include <memory>
#include <stdexcept>
#include <array>

namespace renderer {
    class Application {
    public:
        static constexpr int WIDTH = 800, HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator = (const Application&) = delete;
        void run();

    private:
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffers(int imageIndex);

        Window window{WIDTH, HEIGHT, "Vulkan"};
        Device device{window};
        std::unique_ptr<SwapChain> swapChain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;

        std::unique_ptr<Model> model;
    };
}


#endif //VULKANLEARN_APPLICATION_H
