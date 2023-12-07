#ifndef RENDERCRAFT_PRIMITIVE_H
#define RENDERCRAFT_PRIMITIVE_H

#include <Core/Material.h>
#include <Core/BVH.h>
#include <Core/light.h>
#include <Core/medium.h>
#include <Core/record.h>
#include <Core/shape.h>
#include <Core/transform.h>
#include <Core/core.h>
#include <Core/log.h>
#include <Core/memory.h>
#include <memory>
#include <variant>
#include <vector>

namespace rdcraft {
class Primitive {
  public:
    virtual AABB getAABB() const = 0;
    virtual Material* getMaterial() const = 0;
    virtual Light* getLight() const = 0;
    virtual bool intersect(const Ray& ray, SurfaceRecord* pRec) const = 0;
    /// mainly used for visibility testing
    virtual bool intersect(const Ray& ray) const = 0;
    virtual bool isLight() const = 0;
    virtual ~Primitive() = default;
};

class GeometryPrimitive : public Primitive {
  private:
    std::unique_ptr<Shape> shape;
    std::variant<MediumInterface, Material*, Light*> surface;

  public:
    GeometryPrimitive(std::unique_ptr<Shape> shape, Material* material)
      : shape(std::move(shape)), surface(material) {
    }
    GeometryPrimitive(std::unique_ptr<Shape> shape, Light* light)
      : shape(std::move(shape)), surface(light) {
    }
    GeometryPrimitive(std::unique_ptr<Shape> shape, MediumInterface interface)
      : shape(std::move(shape)), surface(interface) {
    }
    /**
     * forward the intersection request to the shape
     */
    // implement some constructors
    bool intersect(const Ray& ray, SurfaceRecord* pRec) const override {
      if (!shape->intersect(ray, pRec)) return false;
      pRec->pr = static_cast<const Primitive*>(this);
      return true;
    }
    bool intersect(const Ray& ray) const override {
      return shape->intersect(ray);
    }
    bool isLight() const override {
      return surface.index() == 1;
    }
    int externalMediumId() const {
      if (surface.index() != 2)
        ERROR("cannot be called on non-medium primitive.");
      return std::get<MediumInterface>(surface).external_id;
    }
    int internalMediumId() const {
      if (surface.index() != 2)
        ERROR("cannot be called on non-medium primitive.")
      return std::get<MediumInterface>(surface).internal_id;
    }
    Material* getMaterial() const override {
      return std::get<Material*>(surface);
    }
    Light* getLight() const override { return std::get<Light*>(surface); }
    AABB getAABB() const override {
      return shape->getAABB();
    }
    virtual ~GeometryPrimitive() = default;
};

class TransformedPrimitive : Primitive {
  private:
    Primitive* pr{};
    Transform* World2Pr{};
    Transform* Pr2World{};

  public:
    bool intersect(const Ray& ray, SurfaceRecord* pRec) const override {
      bool hasIntersection = pr->intersect(World2Pr->apply(ray), pRec);
      if (hasIntersection) {
        pRec->normal = Pr2World->transNormal(pRec->normal);
        pRec->pos = Pr2World->apply(pRec->pos);
      }
      return hasIntersection;
    }
    bool intersect(const Ray& ray) const override {
      return pr->intersect(World2Pr->apply(ray));
    }
    Material* getMaterial() const override { return pr->getMaterial(); }
    Light* getLight() const override { return pr->getLight(); }
    bool isLight() const override { return pr->isLight(); };
};

class Aggregate : public Primitive {
  private:
    std::unique_ptr<LBVH> lbvh;
    MemoryManager<Primitive> primitives;

  public:
    explicit Aggregate(MemoryManager<Primitive>&& primitive_lists)
      : primitives(std::move(primitive_lists)) {
      lbvh = std::make_unique<LBVH>(primitives);
    }
    Material* getMaterial() const override {
      ERROR("function shouldn't be called.");
    }
    Light* getLight() const override { ERROR("function shouldn't be called."); }
    bool isLight() const override { ERROR("function shouldn't be called."); }
    bool intersect(const Ray& ray, SurfaceRecord* pRec) const override {
      return lbvh->intersect(ray, pRec);
    }
    bool intersect(const Ray& ray) const override {
      return lbvh->intersect(ray);
    }
    AABB getAABB() const override { return lbvh->getAABB(); }
};
} // namespace rdcraft
#endif // RENDERCRAFT_PRIMITIVE_H