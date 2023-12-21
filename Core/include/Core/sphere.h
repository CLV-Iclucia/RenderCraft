#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include <Core/ray.h>
#include <Core/shape.h>
#include <Core/utils.h>

namespace rdcraft {
class Sphere final : public Shape {
  public:
    explicit Sphere(Real r) : R(r) {}
    // another constructor, takes a radius, and two shared_ptr to Transform
    Sphere(Real r, const Transform* World2Obj, const Transform* Obj2World)
      : Shape(World2Obj, Obj2World), R(r) {}
    void intersect(const Ray& ray,
                   std::optional<SurfaceInteraction>& interaction)
    const override;
    bool intersect(const Ray& ray) const override;
    AABB getAABB() const override { return {Vec3{-R, -R, -R}, Vec3{R, R, R}}; }
    ShapeSampleRecord sample(Sampler& sampler) const override {
      Vec3 ret = uniformSampleSphere();
      return {{ret * R, ret}, PI4_INV};
    }
    Real pdfSample(const Vec3& p) const override {
      return PI4_INV;
    }
    Vec3 getNormal(const Vec3& p) const override {
      return normalize(p);
    }
    Real surfaceArea() const override {
      return R * R * PI4;
    }
  private:
    Real R = 0.0;
};
}
#endif