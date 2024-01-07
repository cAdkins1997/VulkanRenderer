
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

namespace rendering{

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator = (const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportCI;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI;
        VkPipelineRasterizationStateCreateInfo rasterizationCI;
        VkPipelineMultisampleStateCreateInfo multisampleCI;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentCI;
        VkPipelineColorBlendStateCreateInfo colorBlendCI;
        VkPipelineDepthStencilStateCreateInfo depthStencilCI;

        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateCI;

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
    public:
        Pipeline(rendering::Device& _device,
                 const std::string& _vertexShader,
                 const std::string &_fragmentShader,
                 const rendering::PipelineConfigInfo& _configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline operator = (const Pipeline&) = delete;

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        void bind(VkCommandBuffer commandBuffer);


    private:
        static std::vector<char> readFile(const std::string& filepath);
        void createGraphicsPipeline(const std::string& _vertexShader,
                                    const std::string& _fragmentShader,
                                    const rendering::PipelineConfigInfo& configInfo);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device &device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;
    };
}

#endif //VULKANLEARN_PIPELINE_H
