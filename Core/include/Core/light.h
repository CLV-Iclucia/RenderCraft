#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include <spectrums.h>
#include <Core/shape.h>
#include <memory>

namespace rdcraft {
class Primitive;
/**
 * Basic light interfaces.
 */
class Light {
  public:
    virtual Spectrum evalEmission(const Patch& pn, const Vec3& wo) const = 0;
    virtual Real pdfSample(const Patch& pn) const = 0;
    virtual Patch sample(Real* pdf) const = 0;
    virtual ~Light() = default;
};

class AreaLight : public Light {
  private:
    const Shape* shape;
    Spectrum radiance;

  public:
    AreaLight(const Shape* shape, const Spectrum& radiance)
      : shape(shape), radiance(radiance) {
    }
    Spectrum evalEmission(const Patch& pn, const Vec3& wo) const override {
      return dot(wo, pn.n) > 0.0 ? radiance : Spectrum();
    }
    Patch sample(Real* pdf) const override;
    Real pdfSample(const Patch& pn) const override {
      return shape->pdfSample(pn.p);
    }
};

class EnvMap {
  public:
    virtual Spectrum evalEmission(const Vec3&) const = 0;
    virtual ~EnvMap() = default;
};

class Sky : public EnvMap {
  private:
    Spectrum ColorA, ColorB;
    // in our setting, the color of the sky will lerp between the two colors.
  public:
    Sky(Spectrum A, Spectrum B)
      : ColorA(std::move(A)), ColorB(std::move(B)) {
    }
    Sky(Real r1, Real g1, Real b1, Real r2, Real g2, Real b2)
      : ColorA({r1, g1, b1}), ColorB({r2, g2, b2}) {
    }
    Spectrum evalEmission(const Vec3&) const override;
};

class PureColor : public EnvMap {
  private:
    Spectrum Color;

  public:
    explicit PureColor(Spectrum C)
      : Color(std::move(C)) {
    }
    explicit PureColor(Real r, Real g, Real b)
      : Color({r, g, b}) {
    }
    Spectrum evalEmission(const Vec3&) const override {
      return Color;
    }
};
}
#endif