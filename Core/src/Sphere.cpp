#include "Core/core.h"
#include <Core/Sphere.h>
#include <Core/maths.h>
#include <cmath>
namespace rdcraft {
bool Sphere::intersect(const Ray &ray, SurfaceRecord *pRec) const {
  Vec3 dif = ray.orig;
  Real B = dot(dif, ray.dir);
  Real C = dot(dif, dif) - R * R;
  Real delta = B * B - C;
  if (delta < 0.0)
    return false;
  delta = std::sqrt(delta);
  if (delta < B)
    return false;
  Real dis = (B + delta) >= 0 ? (delta - B) : -(delta + B);
  Vec3 localPos = ray(dis);
  pRec->uv = Vec2(localPos.x == 0.0 ? PI_DIV_2 : atan(localPos.y / localPos.x) / PI2,
              acos(localPos.z / R));
  pRec->pos = localPos;
  pRec->normal = normalize(Obj2World->transNormal(pRec->pos));
  return true;
}

bool Sphere::intersect(const Ray &ray) const {
  Ray objRay = World2Obj->apply(ray);
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

Patch Sphere::sample(Real* pdf) const {
  *pdf = PI4_INV;
  Vec3 ret = uniformSampleSphere();
  return {ret * R, ret};
}
} // namespace rdcraft
