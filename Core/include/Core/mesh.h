#ifndef RENDERCRAFT_MESH_H
#define RENDERCRAFT_MESH_H

#include <Core/core.h>
#include <Core/shape.h>
#include <Core/utils.h>
#include <vector>

namespace rdcraft {
struct ShapeSampleRecord;
struct Mesh : NonCopyable {
  int triangleCount{};
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> uvs;
  std::vector<int> indices;
  Mesh() = default;
  Mesh(Mesh&& other) noexcept
    : triangleCount(other.triangleCount), vertices(std::move(other.vertices)),
      normals(std::move(other.normals)), uvs(std::move(other.uvs)),
      indices(std::move(other.indices)) {
  }
  Mesh& operator=(Mesh&& rhs) noexcept {
    triangleCount = rhs.triangleCount;
    vertices = std::move(rhs.vertices);
    normals = std::move(rhs.normals);
    uvs = std::move(rhs.uvs);
    indices = std::move(rhs.indices);
    return *this;
  }
};

// this design follows pbrt
struct Triangle final : public Shape {
  Triangle() = default;
  Triangle(const int* idx_, Mesh* mesh_)
    : idx(idx_), mesh(mesh_) {
  }
  void intersect(const Ray& ray,
                 std::optional<SurfaceInteraction>& interaction) const override;
  bool intersect(const Ray& ray) const override;
  AABB getAABB() const override {
    Vec3 lo{std::min(std::min(pos(0).x, pos(1).x), pos(2).x),
            std::min(std::min(pos(0).y, pos(1).y), pos(2).y),
            std::min(std::min(pos(0).z, pos(1).z), pos(2).z)};
    Vec3 hi{std::max(std::max(pos(0).x, pos(1).x), pos(2).x),
            std::max(std::max(pos(0).y, pos(1).y), pos(2).y),
            std::max(std::max(pos(0).z, pos(1).z), pos(2).z)};
    return {lo, hi};
  }
  ShapeSampleRecord sample(Sampler& sampler) const override;
  Vec3 getNormal(const Vec3& p) const override {
    return normalize(cross(pos(1) - pos(0), pos(2) - pos(0)));
  }
  Real pdfSample(const Vec3& p) const override {
    return 1.0 / surfaceArea();
  }
  Real surfaceArea() const override {
    return 0.5 * length(cross(pos(1) - pos(0), pos(2) - pos(0)));
  }
  Vec3 pos(int i) const {
    assert(i >= 0 && i <= 2);
    return mesh->vertices[idx[i]];
  }
  const int* idx = nullptr;
  Mesh* mesh = nullptr; ///< the mesh that the triangle belongs to
};
} // namespace rdcraft

#endif