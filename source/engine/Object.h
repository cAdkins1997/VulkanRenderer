
#ifndef VULKANLEARN_OBJECT_H
#define VULKANLEARN_OBJECT_H

#include "../vulkan/Model.h"

#include <memory>

namespace engine {

    struct Transform2dComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        [[nodiscard]] glm::mat2 mat2() const {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotationMatrix{{c, s}, {-s, c}};
            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotationMatrix * scaleMat;
        }
    };

    using uint32 = unsigned int;

    class Object {
    public:
        explicit Object(uint32 objId) : id{objId} {}

        Object(const Object&) = delete;
        Object & operator = (const Object &) = delete;
        Object(Object&&) = default;
        Object & operator = (Object&&) = default;

        static Object createObject() {
            static uint32 currentId = 0;
            return Object{currentId++};
        };

        uint32 getId() { return id; }

        std::shared_ptr<rendering::Model> model;
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        uint32 id;
    };
};


#endif //VULKANLEARN_OBJECT_H
