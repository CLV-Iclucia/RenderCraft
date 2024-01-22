#include <Core/core.h>
#include <Core/sphere.h>
#include <Core/utils.h>
#include <Core/interaction.h>
#include <cmath>

namespace rdcraft {
void Sphere::intersect(const Ray& ray,
                       std::optional<SurfaceInteraction>& interaction) const {
  Real B = dot(ray.orig, ray.dir);
  Real C = dot(ray.orig, ray.orig) - R * R;
  Real delta = B * B - C;
  if (delta < 0.0)
    return static_cast<void>(interaction = std::nullopt);
  Real sqrt_delta = std::sqrt(delta);
  Real root1 = -B - sqrt_delta;
  Real root2 = -B + sqrt_delta;
  if (root1 > root2)
    std::swap(root1, root2);
  if (root2 < 0.0)
    return static_cast<void>(interaction = std::nullopt);
  Real dis = root1 >= 0.0 ? root1 : root2;
  Vec3 localPos = ray(dis);
  auto uv = Vec2(
      localPos.x == 0.0 ? PI_DIV_2 : atan(localPos.y / localPos.x) / PI2,
      acos(localPos.z / R));
  interaction.emplace(uv, localPos, localPos / R);
}

bool Sphere::intersect(const Ray& ray) const {
  Ray objRay = World2Obj->transform(ray);
  Vec3 dif = objRay.orig;
  Real B = dot(dif, objRay.dir);
  Real C = dot(dif, dif) - R * R;
  Real delta = B * B - C;
  if (delta < 0.0)
    return false;
  delta = std::sqrt(delta);
  if (delta < B)
    return false;
  return true;
}

} // namespace rdcraft