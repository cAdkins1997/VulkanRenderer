
#ifndef VULKANLEARN_PIPELINE_H
#define VULKANLEARN

#include "VulkanCommon.h"
#include "Device.hpp"
#include "Model.h"

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace renderer{

    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI;
        VkPipelineRasterizationStateCreateInfo rasterizationCI;
        VkPipelineMultisampleStateCreateInfo multisampleCI;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentCI;
        VkPipelineColorBlendStateCreateInfo colorBlendCI;
        VkPipelineDepthStencilStateCreateInfo depthStencilCI;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
    public:
        Pipeline(renderer::Device& _device,
                 const std::string& _vertexShader,
                 const std::string &_fragmentShader,
                 const renderer::PipelineConfigInfo& _configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        void operator = (const Pipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        void bind(VkCommandBuffer commandBuffer);


    private:
        static std::vector<char> readFile(const std::string& filepath);
        void createGraphicsPipeline(const std::string& _vertexShader,
                                    const std::string& _fragmentShader,
                                    const renderer::PipelineConfigInfo& configInfo);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device &device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;
    };
}

#endif //VULKANLEARN_PIPELINE_H
