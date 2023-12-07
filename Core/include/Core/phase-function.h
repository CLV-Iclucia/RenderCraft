#ifndef RENDERCRAFT_CORE_PHASE_FUNCTION_H
#define RENDERCRAFT_CORE_PHASE_FUNCTION_H

#include <Core/core.h>
#include <Core/maths.h>

namespace rdcraft {
class PhaseFunction {
  public:
    virtual Real p(const Vec3& wo, const Vec3& wi) const = 0;
    virtual void sample(const Vec3& wo, Vec3* wi, Real* pdf) const = 0;
    virtual Real pdfSample(const Vec3& wo, const Vec3& wi) const = 0;
    virtual ~PhaseFunction() = default;
};

class HenyeyGreenstein : public PhaseFunction {
  public:
    explicit HenyeyGreenstein(Real g)
      : g(g) {
    }
    Real p(const Vec3& wo, const Vec3& wi) const override {
      Real cosTheta = dot(wo, wi);
      return (1.0 - g * g) / (4.0 * PI * pow(1.0 + g * g - 2.0 * g * cosTheta,
                                             1.5));
    }
    void sample(const Vec3& wo, Vec3* wi, Real* pdf) const override;
    Real pdfSample(const Vec3& wo, const Vec3& wi) const override;

  private:
    Real g;
};
}

#endif