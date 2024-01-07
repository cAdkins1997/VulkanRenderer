
#ifndef VULKANLEARN_APPLICATION_H
#define VULKANLEARN_APPLICATION_H

#include "Pipeline.h"
#include "Window.h"
#include "Device.hpp"
#include "Model.h"
#include "Renderer.h"

#include "../engine/Object.h"

#include <memory>
#include <stdexcept>
#include <array>

#include <glm/gtc/constants.hpp>

namespace rendering {
    class Application {
    public:
        static constexpr int WIDTH = 800, HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator = (const Application&) = delete;

        void run();

    private:
        void loadObjects();
        void createPipelineLayout();
        void createPipeline();
        void renderObjects(VkCommandBuffer commandBuffer);

        Window window{WIDTH, HEIGHT, "Vulkan"};
        Device device{window};
        Renderer renderer{window, device};

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        std::vector<engine::Object> objects;
    };
}


#endif //VULKANLEARN_APPLICATION_H
