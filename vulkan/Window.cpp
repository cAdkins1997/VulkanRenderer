
#include "Window.h"

renderer::Window::Window(int _windowWidth, int _windowHeight, std::string _name)
: windowWidth{_windowHeight}, windowHeight{_windowWidth}, name{_name}{
    initWindow();
}

renderer::Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void renderer::Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(windowWidth, windowHeight, name.c_str(), nullptr, nullptr);
}

void renderer::Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface\n");
    }
}