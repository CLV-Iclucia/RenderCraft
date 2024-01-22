//
// Created by creeper on 23-4-17.
//

#ifndef RENDERCRAFT_RD_MATHS_H
#define RENDERCRAFT_RD_MATHS_H
#include <Core/core.h>
#include <Core/ray.h>
#include <glm/gtc/matrix_transform.hpp>

namespace rdcraft {
constexpr Real EPS = 1e-9;
constexpr Real PI = 3.141592653589793;
constexpr Real PI_INV = 0.3183098861837907;
constexpr Real PI_DIV_2 = 1.5707963267948966;
constexpr Real PI4_INV = 0.07957747154594767;
constexpr Real PI2 = 6.283185307179586;
constexpr Real PI2_INV = 0.15915494309189535;
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
static T cross(const T& a, const T& b) {
  return glm::cross(b, a);
}

inline Real distSqr(const Vec3& A, const Vec3& B) { return dot(A - B, A - B); }

template <typename T>
T infinity() {
  return std::numeric_limits<T>::infinity();
}

inline Mat3 constructFrame(const Vec3& N) {
  Vec3 T(1.0, 0.0, 0.0);
  if (N.x >= 0.999 || N.x <= -0.999)
  {
    T.x = 0.0;
    T.y = 1.0;
  }
  Vec3 B = normalize(cross(N, T));
  T = normalize(cross(N, B));
  return Mat3(T, B, N);
}

inline Real computeGeometry(const SurfacePatch& A, const SurfacePatch& B) {
  return std::max(-dot(A.n, B.n), 0.0) / distSqr(A.p, B.p);
}
template <typename T>
T lerp(const T& A, const T& B, Real t) { return A * (1 - t) + B * t; }

template <typename T>
T bilerp(const T& A, const T& B, const T& C, const T& D, Real u,
         Real v) {
  return lerp(lerp(A, B, u), lerp(C, D, u), v);
}

template <typename T>
T trilerp(const T& A, const T& B, const T& C, const T& D, const T& E,
          const T& F, const T& G, const T& H, Real u, Real v, Real w) {
  return lerp(bilerp(A, B, C, D, u, v), bilerp(E, F, G, H, u, v), w);
}

template <typename T, int Dim>
T Max(const Vector<T, Dim>& vec) {
  static_assert(Dim > 0);
  T ret = vec[0];
  for (int i = 1; i < Dim; i++)
    ret = std::max(ret, vec[i]);
  return ret;
}

template <typename T, int Dim>
Vector<T, Dim> max(const Vector<T, Dim>& lhs, const Vector<T, Dim>& rhs) {
  static_assert(Dim > 0);
  Vector<T, Dim> ret;
  for (int i = 0; i < Dim; i++)
    ret[i] = std::max(lhs[i], rhs[i]);
  return ret;
}

template <typename T, int Dim>
Vector<T, Dim> min(const Vector<T, Dim>& lhs, const Vector<T, Dim>& rhs) {
  static_assert(Dim > 0);
  Vector<T, Dim> ret;
  for (int i = 0; i < Dim; i++)
    ret[i] = std::min(lhs[i], rhs[i]);
  return ret;
}

template <typename T, int Dim>
Vector<T, Dim> exp(const Vector<T, Dim>& vec) {
  static_assert(Dim > 0);
  Vector<T, Dim> ret;
  for (int i = 0; i < Dim; i++)
    ret[i] += std::exp(vec[i]);
  return ret;
}

template <typename T, int Dim>
T avg(const Vector<T, Dim>& vec) {
  static_assert(Dim > 0);
  T ret = vec[0];
  for (int i = 1; i < Dim; i++)
    ret += vec[i];
  return ret / Dim;
}

template <typename T>
bool equal(T a, T b) {
  return std::abs(a - b) < EPS;
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

inline Mat4 identity() {
  Mat4 mat;
  mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1;
  return mat;
}

inline bool intersectBBox(const AABB& bbox, const Ray& ray) {
  const Vec3& orig = ray.orig;
  const Vec3& dir = ray.dir;
  const Vec3 invDir = 1.0 / dir;
  const Vec3b dirIsNeg{dir[0] < 0.0, dir[1] < 0.0, dir[2] < 0.0};
  Vec3 tMin = (lo(bbox) - orig) * invDir;
  Vec3 tMax = (hi(bbox) - orig) * invDir;
  if (dirIsNeg[0]) std::swap(tMin[0], tMax[0]);
  if (dirIsNeg[1]) std::swap(tMin[1], tMax[1]);
  if (dirIsNeg[2]) std::swap(tMin[2], tMax[2]);
  Real t_in = std::max(std::max(tMin[0], tMin[1]), tMin[2]);
  if (Real t_out = std::min(std::min(tMax[0], tMax[1]), tMax[2]);
    t_in > t_out || t_out < 0.0)
    return false;
  return true;
}
}
#endif //RENDERCRAFT_RD_MATHS_H