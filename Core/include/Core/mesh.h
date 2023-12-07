#ifndef RENDERCRAFT_MESH_H
#define RENDERCRAFT_MESH_H

#include <Core/core.h>
#include <Core/shape.h>
#include <vector>

namespace rdcraft {
struct Mesh {
  int triangleCount;
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> uvs;
  std::vector<int> indices;
};

// this design follows pbrt
struct Triangle : public Shape {
  Triangle() = default;
  Patch sample(Real *pdf) const override;
  Triangle(const int* idx_, Mesh *mesh_) : idx(idx_), mesh(mesh_) {}
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override;
  bool intersect(const Ray &ray) const override;
  AABB getAABB() const override;
  Real pdfSample(const Vec3 &p) const override;
  Vec3 pos(int i) const {
    assert(i >= 0 && i <= 2);
    return mesh->vertices[idx[i]];
  }
  const int* idx = nullptr;
  Mesh* mesh = nullptr; ///< the mesh that the triangle belongs to
};

}  // namespace rdcraft

#endif