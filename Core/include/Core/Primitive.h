#ifndef RENDERCRAFT_PRIMITIVE_H
#define RENDERCRAFT_PRIMITIVE_H

#include <Core/Material.h>
#include <Core/BVH.h>
#include <Core/Light.h>
#include <Core/Medium.h>
#include <Core/Record.h>
#include <Core/Shape.h>
#include <Core/Transform.h>
#include <Core/core.h>
#include <Core/log.h>
#include <memory>
#include <variant>
#include <vector>
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
  std::unique_ptr<Shape> shape; // all shapes are managed by Primitives!
  std::variant<Material *, Light *> surface;
  MediumInterface interface {-1, -1};
  
public:
  GeometryPrimitive(std::unique_ptr<Shape> shape, Material *material,
                    MediumInterface interface)
      : shape(std::move(shape)), surface(material), interface(interface) {}
  // implement some constructor with default interface
  GeometryPrimitive(std::unique_ptr<Shape> shape, Material *material)
      : shape(std::move(shape)), surface(material) {}
  /**
   * forward the intersection request to the shape
   */
  // implement some constructors
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = shape->intersect(ray, pRec);
    if (hasIntersection)
      pRec->pr = (Primitive *)this;
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override {
    return shape->intersect(ray);
  }
  bool isLight() const override { return surface.index() == 1; }
  int externalMediumId() const { return interface.external_id; }
  int internalMediumId() const { return interface.internal_id; }
  Material *getMaterial() const override {
    return std::get<Material *>(surface);
  }
  Light *getLight() const override { return std::get<Light *>(surface); }
  AABB getAABB() const override {
    return shape->getAABB();
  }
};

class TransformedPrimitive : Primitive {
private:
  Primitive* pr;
  Transform* World2Pr;
  Transform* Pr2World;

public:
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = pr->intersect(World2Pr->apply(ray), pRec);
    if (hasIntersection) {
      pRec->normal = Pr2World->transNormal(pRec->normal);
      pRec->pos = Pr2World->apply(pRec->pos);
    }
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override {
    return pr->intersect(World2Pr->apply(ray));
  }
  Material *getMaterial() const override { return pr->getMaterial(); }
  Light *getLight() const override { return pr->getLight(); }
  bool isLight() const override { return pr->isLight(); };
};

class Aggregate : public Primitive {
private:
  std::unique_ptr<BVH> bvh;

public:
  Aggregate(const std::vector<std::unique_ptr<Primitive>> &primitives)
      : bvh(std::make_unique<BVH>(primitives)) {}
  Material *getMaterial() const override {
    ERROR("function shouldn't be called.");
  }
  Light *getLight() const override { ERROR("function shouldn't be called."); }
  bool isLight() const override { ERROR("function shouldn't be called."); }
  bool intersect(const Ray &ray, SurfaceRecord *pRec) const override {
    bool hasIntersection = bvh->intersect(ray, pRec);
    if (hasIntersection)
      pRec->pr = (Primitive *)(this);
    return hasIntersection;
  }
  bool intersect(const Ray &ray) const override { return bvh->intersect(ray); }
  AABB getAABB() const override { return bvh->getAABB(); }
};
} // namespace rdcraft
#endif // RENDERCRAFT_PRIMITIVE_H
