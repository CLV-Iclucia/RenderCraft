//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <Core/core.h>
#include <utility>
#include <Core/maths.h>
#include <Core/Ray.h>

namespace rdcraft {
struct Transform {
  Mat4 trans; // one matrix to cover all kinds of transforms
  Vec3 operator()(const Vec3& v) const {
    // multiply the matrix with the vector
    Vec4 v4(v.x, v.y, v.z, 1);
    Vec4 result = trans * v4;
    return Vec3(result.x, result.y, result.z);
  }
  Vec3 transNormal(const Vec3& v) const {
    
  }
};
}
#endif //RENDERCRAFT_TRANSFORM_H
