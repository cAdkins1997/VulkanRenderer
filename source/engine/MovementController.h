
#ifndef VULKANLEARN_MOVEMENTCONTROLLER_H
#define VULKANLEARN_MOVEMENTCONTROLLER_H

#include "Object.h"
#include "../vulkan/Window.h"

namespace engine {
    class MovementController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_R;
            int moveDown = GLFW_KEY_F;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int moveSlow = GLFW_KEY_LEFT_SHIFT;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, Object& gameObject);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
    };
}


#endif //VULKANLEARN_MOVEMENTCONTROLLER_H
