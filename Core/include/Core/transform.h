//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <Core/core.h>
#include <utility>
#include <Core/maths.h>
#include <Core/ray.h>

namespace rdcraft {
struct Transform {
  Mat4 trans; // one matrix to rule them all
  Transform() = default;
  explicit Transform(const Mat4& mat)
    : trans(mat) {
  }
  Vec3 operator()(const Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    return Vec3(result.x, result.y, result.z);
  }
  Vec3 apply(const Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    return Vec3(result.x, result.y, result.z);
  }
  Ray apply(const Ray& ray) const {
    return Ray(apply(ray.orig), apply(ray.dir));
  }
  Transform inverse() const {
    return Transform(glm::inverse(trans));
  }
  Vec3 transNormal(const Vec3& v) const {
    // apply transform to normal
  }
};
}
#endif //RENDERCRAFT_TRANSFORM_H