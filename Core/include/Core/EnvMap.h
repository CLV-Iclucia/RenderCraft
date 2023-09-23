//
// Created by creeper on 22-10-28.
//

#ifndef RENDERCRAFT_ENVMAP_H
#define RENDERCRAFT_ENVMAP_H

#include <Core/Light.h>
#include <Core/core.h>

namespace rdcraft {
class EnvMap {
 public:
  virtual Spectrum evalEmission(const Vec3 &) const = 0;
};

class Sky : public EnvMap {
 private:
  Spectrum ColorA, ColorB;// in our setting, the color of the sky will lerp between the two colors.
 public:
  Sky(Spectrum A, Spectrum B) : ColorA(std::move(A)), ColorB(std::move(B)) {}
  Sky(Real r1, Real g1, Real b1, Real r2, Real g2, Real b2) : ColorA({r1, g1, b1}), ColorB({r2, g2, b2}) {}
  Spectrum evalEmission(const Vec3 &) const override;
};

class PureColor : public EnvMap {
  private:
    Spectrum Color;
  public:
    explicit PureColor(Spectrum C) : Color(std::move(C)) {}
    explicit PureColor(Real r, Real g, Real b) : Color({r, g, b}) {}
    Spectrum evalEmission(const Vec3 &) const override {
      return Color;
    }
  };
}
#endif //RENDERCRAFT_ENVMAP_H
