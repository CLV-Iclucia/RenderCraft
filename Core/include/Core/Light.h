#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include <Core/Spectrums.h>
#include <Core/Shape.h>
#include <memory>

namespace rdcraft {
class Primitive;
/**
 * Basic light interfaces.
 */
class Light {
 public:
  virtual Spectrum evalEmission(const Patch &pn, const Vec3 &wo) const = 0;
  virtual Real pdfSample(const Patch &pn) const = 0;
  virtual Patch sample(Real *pdf) const = 0;
};

class AreaLight : public Light {
 private:
  Shape* shape;
  Spectrum radiance;
 public:
  AreaLight(Shape* shape, Spectrum radiance)
      : shape(std::move(shape)), radiance(radiance) {}
  Spectrum evalEmission(const Patch &pn, const Vec3 &wo) const override {
    return dot(wo, pn.n) > 0.0 ? radiance : Spectrum();
  }
  Patch sample(Real* pdf) const override;
  Real pdfSample(const Patch &pn) const override { return shape->pdfSample(pn.p); }
};
}
#endif