
#include "Application.h"
#include "Camera.h"
#include "../engine/MovementController.h"

namespace rendering {

    rendering::Application::Application() {
        loadObjects();
    }

    rendering::Application::~Application() = default;

    void rendering::Application::run() {
        RenderSystem renderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};
        float aspect = renderer.getAspectRatio();
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 2.0f));

        auto viewerObject = engine::Object::createObject();
        engine::MovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!window.shouldCLose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderObjects(commandBuffer, objects, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.device());
    }

    void rendering::Application::loadObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../models/colored_cube.obj");
        auto object = engine::Object::createObject();
        object.model = model;
        object.transform.translation = {0.0f, 0.5f, 2.5f};
        object.transform.scale = {glm::vec3(1.0f)};
        objects.push_back(std::move(object));
    }
};