//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <Core/core.h>
#include <Core/utils.h>
#include <Core/ray.h>
#include <Core/debug.h>
#include <utility>

namespace rdcraft {
struct Transform {
  Mat4 trans; // one matrix to rule them all
  Transform() = default;
  explicit Transform(const Mat4& mat)
    : trans(mat) {
  }
  static Transform translate(const Vec3& v) {
    return Transform(glm::translate(Mat4(1), v));
  }
  Vec3 operator()(const Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    CHECK_NEZ(result.w);
    return Vec3(result.x / result.w, result.y / result.w, result.z / result.w);
  }
  Vec3 apply(const Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    CHECK_NEZ(result.w);
    return Vec3(result.x / result.w, result.y / result.w, result.z / result.w);
  }
  void transform(Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    CHECK_NEZ(result.w);
    v = Vec3(result.x / result.w, result.y / result.w, result.z / result.w);
  }
  SurfacePatch transform(const SurfacePatch& patch) const {
    return {apply(patch.p), transNormal(patch.n)};
  }
  AABB transform(const AABB& aabb) const {
    Vec3 transformed_lo = apply(lo(aabb));
    Vec3 transformed_hi = apply(hi(aabb));
    Vec3 real_lo = min<Real, 3>(transformed_lo, transformed_hi);
    Vec3 real_hi = max<Real, 3>(transformed_lo, transformed_hi);
    return {real_lo, real_hi};
  }
  Ray transform(const Ray& ray) const {
    Vec3 transformed_orig = apply(ray.orig);
    // remove the affine part of the transform
    Vec3 transformed_dir = apply(ray.orig + ray.dir) - transformed_orig;
    return {transformed_orig, normalize(transformed_dir)};
  }
  Transform inverse() const {
    return Transform(glm::inverse(trans));
  }
  Vec3 transNormal(const Vec3& v) const {
    Vec4 v4(v.x, v.y, v.z, 0);
    Mat4 invT = transpose(glm::inverse(trans));
    Vec4 result = invT * v4;
    return Vec3(result.x, result.y, result.z);
  }
};


inline Transform translate(const Vec3& v) {
  return Transform(glm::translate(identity(), v));
}

inline Transform scale(const Vec3& v) {
  return Transform(glm::scale(identity(), v));
}

inline Transform rotate(Real angle, const Vec3& axis) {
  return Transform(glm::rotate(identity(), angle, axis));
}

}
#endif //RENDERCRAFT_TRANSFORM_H