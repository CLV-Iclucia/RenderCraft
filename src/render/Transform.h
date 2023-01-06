//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <utility>

#include "../../XMath/core/core.h"
#include "../../XMath/ext/Graphics/MathUtils.h"

/**
 * first rotate, then translate
 */
struct Transform
{
    Mat3x3<Real> rot;
    Vec3 translate;
    Transform() = default;
    Transform(const Transform& _transform) = default;
    Transform(Mat3&& _rot, Vec3 _translate) : rot(_rot), translate(std::move(_translate)) {}
    Transform(const Mat3& _rot, Vec3 _translate) : rot(_rot), translate(std::move(_translate)) {}
    Transform inv() const;
    Vec3 operator ()(const Vec3& v) const { return rot * v + translate; }
};
#endif //RENDERCRAFT_TRANSFORM_H
