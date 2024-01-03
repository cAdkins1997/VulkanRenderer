
#ifndef VULKANLEARN_WINDOW_H
#define VULKANLEARN_WINDOW_H

#include "VulkanCommon.h"

#include <string>
#include <stdexcept>

namespace renderer {
    class Window {
    public:
        Window(int _windowWidth, int _windowHeight, std::string _name);
        ~Window();

        Window(const Window&) = delete;
        Window &operator = (const Window&) = delete;

        bool shouldCLose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return {static_cast<uint32_t>(windowWidth),
                                         static_cast<uint32_t>(windowHeight)
        }; }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        void initWindow();

        const int windowWidth, windowHeight;

        std::string name;
        GLFWwindow* window;
    };
}

#endif //VULKANLEARN_WINDOW_H
