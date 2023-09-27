#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include <Core/Material.h>
#include <Core/Shape.h>

namespace rdcraft {
struct Mesh;
struct Triangle : public Shape {
  Triangle() = default;
  Patch sample(Real *pdf) const override;
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override;
  bool intersect(const Ray &ray) const override;
  AABB getAABB() const override;
  Real pdfSample(const Vec3 &p) const override;
  Real pdfSample(const Vec3 &p, const Vec3 &ref) const override;
  int v[3];
  int uv[3];
  int n[3];
  std::shared_ptr<Mesh> mesh =
      nullptr; ///< the mesh that the triangle belongs to
};
} // namespace rdcraft
#endif