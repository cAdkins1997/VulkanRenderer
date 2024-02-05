
#ifndef VULKANLEARN_OBJECT_H
#define VULKANLEARN_OBJECT_H

#include "../vulkan/Model.h"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace engine {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        [[nodiscard]] glm::mat4 mat4() const;
        [[nodiscard]] glm::mat3 normalMatrix() const;
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
        TransformComponent transform{};

    private:
        uint32 id;
    };
};


#endif //VULKANLEARN_OBJECT_H
