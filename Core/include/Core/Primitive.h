//
// Created by creeper on 23-4-11.
//

#ifndef RENDERCRAFT_PRIMITIVE_H
#define RENDERCRAFT_PRIMITIVE_H

#include <Core/Light.h>
#include <Core/Medium.h>
#include <Core/Record.h>
#include <Core/Shape.h>
#include <Core/Transform.h>
#include <Core/core.h>
#include <variant>
#include <vector>
#include <Core/log.h>
namespace rdcraft {
class Primitive {
 public:
  virtual AABB getAABB() const = 0;
  virtual Material *getMaterial() const = 0;
  virtual Light *getLight() const = 0;
  virtual bool intersect(const Ray &ray, SurfaceRecord *pRec) const = 0;
  /// mainly used for visibility testing
  virtual bool intersect(const Ray &ray) const = 0;
  virtual bool isLight() const = 0;
};

class GeometryPrimitive : public Primitive {
 private:
  std::shared_ptr<Shape> shape;
  std::variant<Material *, Light *> surface;
  MediumInterface interface;
 public:
  /**
   * forward the intersection request to the shape
   */
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = shape->intersect(ray, pRec);
    if (hasIntersection) pRec->pr = (Primitive *) this;
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override { return shape->intersect(ray); }
  bool isLight() const override { return surface.index() == 1; }
  int externalMediumId() const { return interface.external_id; }
  int internalMediumId() const { return interface.internal_id; }
  Material *getMaterial() const override { return std::get<Material *>(surface); }
  Light *getLight() const override { return std::get<Light *>(surface); }
};

class TransformedPrimitive : Primitive {
 private:
  std::shared_ptr<Primitive> pr;
  Transform World2Pr, Pr2World;
 public:
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = pr->intersect(World2Pr(ray), pRec);
    if (hasIntersection) {
      pRec->normal = Pr2World.transNormal(pRec->normal);
      pRec->pos = Pr2World(pRec->pos);
    }
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override { return pr->intersect(World2Pr(ray)); }
  Material *getMaterial() const override { return pr->getMaterial(); }
  Light *getLight() const override { return pr->getLight(); }
  bool isLight() const override { return pr->isLight(); };
};

class Aggregate : public Primitive {
 private:
  std::vector<Primitive *> primitives;
  std::shared_ptr<BVH> bvh;
 public:
  Material *getMaterial() const override {
    ERROR("function shouldn't be called.");
  }
  Light *getLight() const override {
    ERROR("function shouldn't be called.");
  }
  bool isLight() const override {
    ERROR("function shouldn't be called.");
  }
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = bvh->intersect(ray, pRec);
    if (hasIntersection) pRec->pr = (Primitive *) (this);
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override { return bvh->intersect(ray); }
  AABB getAABB() const override { return bvh->getAABB(); }
};
}
#endif //RENDERCRAFT_PRIMITIVE_H
