
#ifndef VULKANLEARN_RENDERSYSTEM_H
#define VULKANLEARN_RENDERSYSTEM_H

#include "Pipeline.h"
#include "Device.hpp"
#include "Model.h"
#include "Camera.h"

#include "../engine/Object.h"

#include <memory>
#include <stdexcept>
#include <array>

#include <glm/gtc/constants.hpp>

namespace rendering {
    class RenderSystem {
    public:

        RenderSystem(Device& _device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator = (const RenderSystem&) = delete;

        void renderObjects(VkCommandBuffer commandBuffer, std::vector<engine::Object>& objects, const Camera& camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        Device& device;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}


#endif //VULKANLEARN_RENDERSYSTEM_H
