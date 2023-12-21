#ifndef RENDERCRAFT_PRIMITIVE_H
#define RENDERCRAFT_PRIMITIVE_H

#include <Core/material.h>
#include <Core/bvh.h>
#include <Core/light.h>
#include <Core/medium.h>
#include <Core/interaction.h>
#include <Core/shape.h>
#include <Core/transform.h>
#include <Core/core.h>
#include <Core/debug.h>
#include <Core/memory.h>
#include <memory>
#include <optional>
#include <variant>

namespace rdcraft {
class Primitive {
  public:
    virtual AABB getAABB() const = 0;
    virtual Material* getMaterial() const = 0;
    virtual Light* getLight() const = 0;
    virtual void intersect(const Ray& ray,
                           std::optional<SurfaceInteraction>& interaction) const
    = 0;
    /// mainly used for visibility testing
    virtual bool intersect(const Ray& ray) const = 0;
    virtual bool isLight() const = 0;
    virtual bool isMediumInterface() const = 0;
    virtual ~Primitive() = default;
};

class GeometryPrimitive : public Primitive {
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
    void intersect(const Ray& ray,
                   std::optional<SurfaceInteraction>& interaction)
    const override {
      shape->intersect(ray, interaction);
      if (!interaction) return;
      interaction->pr = static_cast<const Primitive*>(this);
    }
    bool intersect(const Ray& ray) const override {
      return shape->intersect(ray);
    }
    bool isLight() const override {
      return surface.index() == 2;
    }
    bool isMediumInterface() const override {
      return surface.index() == 0;
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

  private:
    std::unique_ptr<Shape> shape;
    std::variant<MediumInterface, Material*, Light*> surface;
};

class TransformedPrimitive : Primitive {
  private:
    Primitive* pr{};
    Transform* World2Pr{};
    Transform* Pr2World{};

  public:
    void intersect(const Ray& ray,
                   std::optional<SurfaceInteraction>& interaction)
    const override {
      pr->intersect(World2Pr->apply(ray), interaction);
      if (interaction) {
        interaction->normal = Pr2World->transNormal(interaction->normal);
        interaction->pos = Pr2World->apply(interaction->pos);
      }
    }
    bool intersect(const Ray& ray) const override {
      return pr->intersect(World2Pr->apply(ray));
    }
    Material* getMaterial() const override { return pr->getMaterial(); }
    Light* getLight() const override { return pr->getLight(); }
    bool isLight() const override { return pr->isLight(); };
};

class Aggregate : public Primitive {
  public:
    explicit Aggregate(MemoryManager<Primitive>&& primitives)
      : lbvh(std::make_unique<LBVH>(primitives)) {}
    Material* getMaterial() const override {
      ERROR("function shouldn't be called.");
    }
    Light* getLight() const override { ERROR("function shouldn't be called."); }
    bool isLight() const override { ERROR("function shouldn't be called."); }
    bool isMediumInterface() const override { ERROR("function shouldn't be called."); }
    void intersect(const Ray& ray,
                   std::optional<SurfaceInteraction>& interaction)
    const override {
      lbvh->intersect(ray, interaction);
    }
    bool intersect(const Ray& ray) const override {
      return lbvh->intersect(ray);
    }
    AABB getAABB() const override { return lbvh->getAABB(); }

  private:
    std::unique_ptr<LBVH> lbvh;
};
} // namespace rdcraft
#endif // RENDERCRAFT_PRIMITIVE_H