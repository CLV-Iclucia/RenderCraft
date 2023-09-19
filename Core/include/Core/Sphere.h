#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include "Ray.h"
#include "Record.h"
#include "Shape.h"
/**
 * Because all the coordinates are in local frame, the position of the sphere is
 * on (0, 0, 0) by default
 */
class Sphere : public Shape {
public:
  explicit Sphere(Real r) : R(r) {}
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override;
  bool intersect(const Ray &ray) const override;
  BBox3 getBBox() const override { return {Vec3{-R, -R, -R}, Vec3{R, R, R}}; }
  Patch sample(Real *pdf) const override;
  Patch sample(const Vec3 &p, Real *pdf) const override;
  Patch sample(const Vec3 &p) const override;

private:
  Real R = 0.0;
};
#endif