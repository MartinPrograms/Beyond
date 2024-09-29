//
// Created by marti on 29/09/2024.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Graphics {
namespace Other {

class Transform {
public:
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;
  glm::mat4 getModelMatrix() const;
};

} // Other
} // Graphics

#endif //TRANSFORM_H
