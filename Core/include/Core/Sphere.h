#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include <Core/Ray.h>
#include <Core/Record.h>
#include <Core/Shape.h>
/**
 * Because all the coordinates are in local frame, the position of the sphere is
 * on (0, 0, 0) by default
 */
namespace rdcraft {
class Sphere : public Shape {
 public:
  explicit Sphere(Real r) : R(r) {}
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override;
  bool intersect(const Ray &ray) const override;
  AABB getAABB() const override { return {Vec3{-R, -R, -R}, Vec3{R, R, R}}; }
  Patch sample(Real *pdf) const override;

 private:
  Real R = 0.0;
};
}
#endif