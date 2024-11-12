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
  Transform() : position(0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f) {}
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;



  glm::mat4 getModelMatrix() const;

  void AddYaw(float x);
  void AddPitch(float y);
  void AddRoll(float z);
};

} // Other
} // Graphics

#endif //TRANSFORM_H
