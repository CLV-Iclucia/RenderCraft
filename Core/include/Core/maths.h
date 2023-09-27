//
// Created by creeper on 23-4-17.
//

#ifndef RENDERCRAFT_RD_MATHS_H
#define RENDERCRAFT_RD_MATHS_H
#include <Core/core.h>
#include <glm/gtc/matrix_transform.hpp>
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
using glm::length;
using glm::refract;
using glm::cross;
using glm::clamp;
using glm::translate;
using glm::rotate;
using glm::scale;
inline Real distSqr(const Vec3 &A, const Vec3 &B) { return dot(A - B, A - B); }
Vec3 uniformSampleSphere() {

}
Vec3 cosWeightedSampleHemisphere() {
    
}
Mat3 constructFrame(const Vec3 &N) {

}

inline Real geometry(const Patch &A, const Patch &B) { return std::max(-dot(A.n, B.n), 0.0) / distSqr(A.p, B.p); }
template<typename T>
inline T lerp(const T &A, const T &B, Real t) { return A * (1 - t) + B * t; }
// bilerp
template<typename T>
inline T bilerp(const T &A, const T &B, const T &C, const T &D, Real u, Real v) {
    return lerp(lerp(A, B, u), lerp(C, D, u), v);
}
// trilerp
template<typename T>
inline T trilerp(const T &A, const T &B, const T &C, const T &D, const T &E, const T &F, const T &G, const T &H, Real u, Real v, Real w) {
    return lerp(bilerp(A, B, C, D, u, v), bilerp(E, F, G, H, u, v), w);
}
}
#endif //RENDERCRAFT_RD_MATHS_H
