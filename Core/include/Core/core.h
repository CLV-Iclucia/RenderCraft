//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_TYPES_H
#define RENDERCRAFT_TYPES_H

#include <glm/glm.hpp>
namespace rdcraft {
using uint = unsigned int;
using Real = double;
using Mat3 = glm::highp_mat3;
using Vec2f = glm::vec2;
using Vec2 = glm::highp_vec2;
using Vec3 = glm::highp_vec3;
using Vec3b = glm::bvec3;
using Vec3f = glm::vec3;
using Vec3i = glm::ivec3;
using AABB = std::tuple<Vec3, Vec3>;
struct Patch {
  Vec3 p;
  Vec3 n;
};
}
#endif //RENDERCRAFT_TYPES_H
