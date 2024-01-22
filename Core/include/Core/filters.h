//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_FILTER_H
#define RENDERCRAFT_FILTER_H

#include <Core/core.h>
#include <Core/utils.h>
#include <Core/sampler.h>

namespace rdcraft {
class Filter {
  public:
    virtual Real eval(Real x, Real y) const = 0;
    virtual Vec2 sample(Sampler& sampler) const = 0;
    virtual ~Filter() = default;
};

class BoxFilter final : public Filter {
  public:
    BoxFilter(Real rx_, Real ry_)
      : rx(rx_), ry(ry_) {
    }
    explicit BoxFilter(const Vec2& window)
      : rx(window.x), ry(window.y) {
    }
    explicit BoxFilter(Real window)
      : rx(window), ry(window) {
    }
    Real eval(Real x, Real y) const override {
      return 1.0;
    }
    Vec2 sample(Sampler& sampler) const override {
      return Vec2(randomReal(sampler, -rx, rx), randomReal(sampler, -ry, ry));
    }

  protected:
    Real rx, ry;
};

// follows pbrt
class GaussianFilter final : public Filter {
  public:
    GaussianFilter(Real alpha, Real rx, Real ry)
      : alpha(alpha), rx(rx), ry(ry) {
    }
    Real eval(Real x, Real y) const override {
      Real gx = gauss(alpha, x, rx);
      Real gy = gauss(alpha, y, ry);
      return gx * gy;
    }
    Vec2 sample(Sampler& sampler) const override {
    }

  private:
    Real alpha = 1.0;
    Real rx = 1.0, ry = 1.0;
};
} // namespace rdcraft
#endif // RENDERCRAFT_FILTER_H