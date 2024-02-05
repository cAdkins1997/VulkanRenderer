
#ifndef VULKANLEARN_APPLICATION_H
#define VULKANLEARN_APPLICATION_H

#include "Window.h"
#include "Device.hpp"
#include "Model.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "FrameInfo.h"
#include "Descriptor.h"

#include "../engine/Object.h"

#include <memory>
#include <stdexcept>
#include <array>
#include <chrono>

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

        Window window{WIDTH, HEIGHT, "Vulkan"};
        Device device{window};
        Renderer renderer{window, device};

        std::unique_ptr<DescriptorPool> globalPool{};
        engine::Object::Map objects;
    };
}


#endif //VULKANLEARN_APPLICATION_H
