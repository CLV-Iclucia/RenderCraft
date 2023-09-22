//
// Created by creeper on 22-11-20.
//
#include <Core/Triangle.h>
namespace rdcraft {
bool Triangle::intersect(const Ray &ray, SurfaceRecord *pRec) const {
  return false;
}
bool Triangle::intersect(const Ray &ray) const { return false; }

AABB Triangle::getAABB() const { return AABB(); }
Real Triangle::pdfSample(const Vec3 &p) const { return 0.0; }
Real Triangle::pdfSample(const Vec3 &p, const Vec3 &ref) const { return 0.0; }

} // namespace rdcraft
