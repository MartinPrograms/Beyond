//
// Created by marti on 29/09/2024.
//

#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Graphics {
namespace Other {
    glm::mat4 Transform::getModelMatrix() const {
        // rotation is quaternion
        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }
} // Other
} // Graphics