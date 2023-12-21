//
// Created by creeper on 22-11-14.
//
#ifndef RENDERCRAFT_SHAPE_H
#define RENDERCRAFT_SHAPE_H
#include <memory>
#include <utility>
#include <Core/ray.h>
#include <Core/transform.h>
#include <Core/core.h>

#include <optional>

namespace rdcraft {
struct SurfaceInteraction;
struct Sampler;
struct ShapeSampleRecord {
  SurfacePatch pn;
  Real pdf{};
};
/*
 * All shapes are defined in object coordinate space.
 * So a transform is needed
 * */
class Shape {
  protected:
    const Transform* World2Obj{};
    const Transform* Obj2World{};

  public:
    Shape() = default;
    // implement some constructors
    Shape(const Transform* world2obj, const Transform* obj2world)
      : World2Obj(world2obj), Obj2World(obj2world) {
    }
    /**
     * sample a point on the surface of the shape given the position of the shape
     * @return the coordinates of the sample point
     */
    virtual ShapeSampleRecord sample(Sampler& sampler) const = 0;
    virtual void intersect(const Ray& ray,
                           std::optional<SurfaceInteraction>& interaction) const
    = 0;
    virtual bool intersect(const Ray& ray) const = 0;
    virtual AABB getAABB() const = 0;
    virtual Real pdfSample(const Vec3& p) const = 0;
    virtual Real surfaceArea() const = 0;
    virtual Vec3 getNormal(const Vec3& p) const = 0;
    virtual ~Shape() = default;
};
}

#endif // RENDERCRAFT_SHAPE_H