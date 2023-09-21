#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include <Core/Material.h>
#include <Core/Shape.h>
#include <Core/Mesh.h>

namespace rdcraft {
class Triangle : public Shape
{
 public:
  Triangle() = default;
  Patch sample(Real *pdf) const override;
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override;
  bool intersect(const Ray &ray) const override;
  AABB getAABB() const override;
  Real pdfSample(const Vec3 &p) const override;
  Real pdfSample(const Vec3 &p, const Vec3 &ref) const override;
 private:
  Vec3 v[3];
  Vec2 uv[3];
  Vec3 n[3];
  std::shared_ptr<Mesh> mesh = nullptr;///< the mesh that the triangle belongs to
};
}
#endif