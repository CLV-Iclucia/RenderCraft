//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_TYPES_H
#define RENDERCRAFT_TYPES_H

#include <glm/glm.hpp>
#include <tuple>
namespace rdcraft {
using uint = unsigned int;
using Real = double;
using Mat3 = glm::highp_mat3;
using Mat4 = glm::highp_mat4;
using Vec2f = glm::vec2;
using Vec2 = glm::highp_vec2;
using Vec3 = glm::highp_vec3;
using Vec4 = glm::highp_vec4;
using Vec3b = glm::bvec3;
using Vec3f = glm::vec3;
using Vec3i = glm::ivec3;
using AABB = std::tuple<Vec3, Vec3>;
template <typename T, int Dim> struct TVector {
  static_assert(Dim == 2 || Dim == 3 || Dim == 4, "Dim must be 2, 3 or 4");
};
template <typename T> struct TVector<T, 2> {
  using type = glm::detail::tvec2<T>;
};
template <typename T> struct TVector<T, 3> {
  using type = glm::detail::tvec3<T>;
};
template <typename T> struct TVector<T, 4> {
  using type = glm::detail::tvec4<T>;
};
template <typename T, int Dim> using Vector = typename TVector<T, Dim>::type;
inline Vec3& lo(AABB& aabb) {
  return std::get<0>(aabb);
}
inline const Vec3& lo(const AABB& aabb) {
  return std::get<0>(aabb);
}
inline Vec3& hi(AABB& aabb) {
  return std::get<1>(aabb);
}
inline const Vec3& hi(const AABB& aabb) {
  return std::get<1>(aabb);
}
struct Patch {
  Vec3 p;
  Vec3 n;
};
struct NonCopyable {
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};
}
#endif //RENDERCRAFT_TYPES_H
