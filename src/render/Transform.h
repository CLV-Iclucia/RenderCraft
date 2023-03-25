//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <utility>
#include "types.h"

struct Transform
{
    Mat3 rot;
    Real scale = 1.0;
    Transform() = default;
    Transform(const Transform& _transform) = default;
    Transform(Mat3 _rot, Real _scale) : rot(_rot), scale(_scale) {}
    explicit Transform(Mat3 _rot) : rot(_rot) {}
    Transform inv() const;
    Vec3 operator ()(const Vec3& v) const { }
};
#endif //RENDERCRAFT_TRANSFORM_H
