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

namespace rdcraft {
class SurfaceRecord;
/*
 * All shapes are defined in object coordinate space.
 * So a transform is needed
 * */
class Shape {
 protected:
  Transform* world_to_obj;
  Transform* obj_to_world;

 public:
  Shape() = default;
  // implement some constructors
  Shape(Transform* world2obj,
        Transform* obj2world)
      : world_to_obj(world2obj), obj_to_world(obj2world) {}
  /**
   * sample a point on the surface of the shape given the position of the shape
   * @return the coordinates of the sample point
   */
  virtual Patch sample(Real *pdf) const = 0;
  /**
   * calc the intersection with a given ray
   * @param ray a ray in the local space
   * @return an Intersection class recording the info of this intersection
   */
  virtual bool intersect(const Ray &ray, SurfaceRecord *pRec) const = 0;
  virtual bool intersect(const Ray &ray) const = 0;
  virtual AABB getAABB() const = 0;
  /**
   * calc the surface area of the shape
   * @return the are of the surface of the shape
   */
  virtual Real pdfSample(const Vec3 &p) const = 0;
  virtual ~Shape() = default;
};
}

#endif // RENDERCRAFT_SHAPE_H