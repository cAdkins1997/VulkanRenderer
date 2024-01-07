
#ifndef VULKANLEARN_WINDOW_H
#define VULKANLEARN_WINDOW_H

#include "VulkanCommon.h"

#include <string>
#include <stdexcept>

namespace rendering {
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
        [[nodiscard]] bool wasWindowResized() const { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        void initWindow();
        static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);

        int windowWidth, windowHeight;
        bool frameBufferResized = false;

        std::string name;
        GLFWwindow* window;
    };
}

#endif //VULKANLEARN_WINDOW_H
