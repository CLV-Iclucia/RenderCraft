//
// Created by creeper on 23-4-17.
//

#ifndef RENDERCRAFT_RD_MATHS_H
#define RENDERCRAFT_RD_MATHS_H
#include <Core/core.h>
namespace rdcraft {

const Real EPS = 1e-9;
const Real PI = 3.141592653589793;
const Real PI_INV = 0.3183098861837907;
const Real PI_DIV_2 = 1.5707963267948966;
const Real PI4_INV = 0.07957747154594767;
const Real PI2 = 6.283185307179586;

using glm::dot;
using glm::distance;
using glm::inverse;
using glm::transpose;
using glm::normalize;
inline Real distSqr(const Vec3 &A, const Vec3 &B) { return dot(A - B, A - B); }
Vec3 uniformSampleSphere() {

}

Mat3 constructFrame(const Vec3 &N) {

}

inline Real geometry(const Patch &A, const Patch &B) { return std::max(-dot(A.n, B.n), 0.0) / distSqr(A.p, B.p); }
}

#endif //RENDERCRAFT_RD_MATHS_H
