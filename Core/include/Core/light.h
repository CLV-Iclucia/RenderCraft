#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include <Core/spectrums.h>
#include <Core/shape.h>
#include <Core/utils.h>

namespace rdcraft {
class Primitive;
/**
 * Basic light interfaces.
 */
class Light {
  public:
    virtual Spectrum evalEmission(const SurfacePatch& pn, const Vec3& wo) const
    = 0;
    virtual Real power() const = 0;
    virtual Real pdfSample(const SurfacePatch& pn) const = 0;
    virtual ShapeSampleRecord sample(Sampler& sampler) const = 0;
    virtual ~Light() = default;
};

class AreaLight final : public Light {
  public:
    AreaLight(const Shape* shape, const Spectrum& radiance)
      : shape(shape), radiance(radiance) {
    }
    Spectrum
    evalEmission(const SurfacePatch& pn, const Vec3& wo) const override {
      return dot(wo, pn.n) > 0.0 ? radiance : Spectrum();
    }
    ShapeSampleRecord sample(Sampler& sampler) const override {
      return shape->sample(sampler);
    }
    Real pdfSample(const SurfacePatch& pn) const override {
      return shape->pdfSample(pn.p);
    }
    Real power() const override {
      return (radiance.r + radiance.g + radiance.b) * shape->surfaceArea();
    }

  private:
    const Shape* shape;
    Spectrum radiance;
};

class EnvMap : public Light {
  private:
    Real worldRadius = 1e4;
};

class ConstantEnvMap : public EnvMap {
  public:
};
}
#endif