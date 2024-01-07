
#include "Application.h"


namespace rendering {

    struct pushConstantsData {
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    rendering::Application::Application() {
        loadObjects();
        createPipelineLayout();
        createPipeline();
    }

    rendering::Application::~Application() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void rendering::Application::run() {

        while(!window.shouldCLose()) {
            glfwPollEvents();

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderObjects(commandBuffer);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.device());
    }

    void rendering::Application::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(pushConstantsData);

        VkPipelineLayoutCreateInfo pipelineLayoutCI{};
        pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCI.setLayoutCount = 0;
        pipelineLayoutCI.pSetLayouts = nullptr;
        pipelineLayoutCI.pushConstantRangeCount = 1;
        pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layou\n");
        }
    }

    void rendering::Application::createPipeline(){
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
                device,
                "../shaders/shader.vert.spv",
                "../shaders/shader.frag.spv",
                pipelineConfig);
    }

    void rendering::Application::loadObjects() {
        std::vector<Model::Vertex> vertices{
                {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

        auto model = std::make_shared<Model>(device, vertices);

        auto triangle = engine::Object::createObject();
        triangle.model = model;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        objects.push_back(std::move(triangle));
    }

    void Application::renderObjects(VkCommandBuffer commandBuffer) {
        pipeline->bind(commandBuffer);

        for (auto& object : objects) {
            object.transform2d.rotation = glm::mod(object.transform2d.rotation + 0.01f, glm::two_pi<float>());

            pushConstantsData push{};
            push.offset = object.transform2d.translation;
            push.color = object.color;
            push.transform = object.transform2d.mat2();

            vkCmdPushConstants(
                    commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(pushConstantsData),
                    &push);
            object.model->bind(commandBuffer);
            object.model->draw(commandBuffer);
        }
    }
};