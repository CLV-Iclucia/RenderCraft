#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include "Shape.h"
#include "Spectrums.h"
#include "core.h"
#include <memory>

namespace rdcraft {
class Primitive;
/**
 * Basic light interfaces.
 */
class Light {
 public:
  virtual Spectrum evalEmission(const Patch &pn, const Vec3 &wo) const = 0;
  virtual Real pdfSample(const Patch &pos, const Vec3 &ref) const = 0;
  virtual Patch sample(const Vec3 &ref, Real *pdf) const = 0;
};

class AreaLight : public Light {
 private:
  std::shared_ptr<Shape> shape;
  Spectrum radiance;
 public:
  Spectrum evalEmission(const Patch &pn, const Vec3 &wo) const override {
    return glm::dot(wo, pn.n) > 0.0 ? radiance : Spectrum();
  }
  Real pdfSample(const Patch &pn, const Vec3 &ref) const override { return shape->pdfSample(pn.p, ref); }
};
}
#endif