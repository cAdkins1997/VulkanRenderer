
#ifndef VULKANLEARN_FRAMEINFO_H
#define VULKANLEARN_FRAMEINFO_H

#include "VulkanCommon.h"
#include "Camera.h"
#include "../engine/Object.h"

namespace rendering {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        engine::Object::Map &objects;
    };
}
class FrameInfo {

};


#endif //VULKANLEARN_FRAMEINFO_H
