//
// Created by creeper on 23-4-17.
//

#ifndef RENDERCRAFT_RD_MATHS_H
#define RENDERCRAFT_RD_MATHS_H
#include <Core/core.h>
#include <glm/gtc/matrix_transform.hpp>

namespace rdcraft {
constexpr Real EPS = 1e-9;
constexpr Real PI = 3.141592653589793;
constexpr Real PI_INV = 0.3183098861837907;
constexpr Real PI_DIV_2 = 1.5707963267948966;
constexpr Real PI4_INV = 0.07957747154594767;
constexpr Real PI2 = 6.283185307179586;
constexpr Real PI4 = 12.566370614359172;

using glm::dot;
using glm::distance;
using glm::inverse;
using glm::transpose;
using glm::normalize;
using glm::length;
using glm::refract;
using glm::clamp;
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::inverse;

template <typename T>
static inline T cross(const T& a, const T& b) {
  return glm::cross(b, a);
}

inline Real distSqr(const Vec3& A, const Vec3& B) { return dot(A - B, A - B); }

template <typename T>
inline T epsilon() {
  if constexpr (std::is_same_v<T, Real>)
    return 1e-9;
  else if constexpr (std::is_same_v<T, float>)
    return 1e-5;
  else return static_cast<T>(0);
}

Vec3 uniformSampleSphere() {
}

Vec3 cosWeightedSampleHemisphere() {
}

Mat3 constructFrame(const Vec3& N) {
}

inline Real computeGeometry(const SurfacePatch& A, const SurfacePatch& B) {
  return std::max(-dot(A.n, B.n), 0.0) / distSqr(A.p, B.p);
}
template <typename T>
inline T lerp(const T& A, const T& B, Real t) { return A * (1 - t) + B * t; }

// bilerp
template <typename T>
inline T bilerp(const T& A, const T& B, const T& C, const T& D, Real u,
                Real v) {
  return lerp(lerp(A, B, u), lerp(C, D, u), v);
}

// trilerp
template <typename T>
inline T trilerp(const T& A, const T& B, const T& C, const T& D, const T& E,
                 const T& F, const T& G, const T& H, Real u, Real v, Real w) {
  return lerp(bilerp(A, B, C, D, u, v), bilerp(E, F, G, H, u, v), w);
}

template <typename T>
inline T clamp(T x, T lo, T hi) {
  return std::max(lo, std::min(x, hi));
}

template <typename T>
inline bool equal(const T& a, const T& b) {
  return std::abs(a - b) < epsilon<T>();
}

inline AABB mergeAABB(const AABB& lhs, const AABB& rhs) {
  return {Vec3(std::min(lo(lhs).x, lo(rhs).x),
               std::min(lo(lhs).y, lo(rhs).y),
               std::min(lo(lhs).z, lo(rhs).z)),
          Vec3(std::max(hi(lhs).x, hi(rhs).x),
               std::max(hi(lhs).y, hi(rhs).y),
               std::max(hi(lhs).z, hi(rhs).z))};
}

inline Real gauss(Real alpha, Real x, Real r) {
  if (std::abs(x) >= r) return 0.0;
  return std::exp(alpha * x * x) - std::exp(alpha * r * r);
}
}
#endif //RENDERCRAFT_RD_MATHS_H