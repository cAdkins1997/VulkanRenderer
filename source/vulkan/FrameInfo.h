
#ifndef VULKANLEARN_FRAMEINFO_H
#define VULKANLEARN_FRAMEINFO_H

#include "Camera.h"
#include "VulkanCommon.h"

namespace rendering {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}
class FrameInfo {

};


#endif //VULKANLEARN_FRAMEINFO_H
