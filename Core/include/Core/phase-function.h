#ifndef RENDERCRAFT_CORE_PHASE_FUNCTION_H
#define RENDERCRAFT_CORE_PHASE_FUNCTION_H

#include <Core/core.h>
#include <Core/utils.h>
#include <Core/sampler.h>
#include <Core/debug.h>
#include <optional>

namespace rdcraft {
struct PhaseFunctionSampleRecord {
  Vec3 wi;
  Real pdf;
};

class PhaseFunction {
  public:
    virtual Real p(const Vec3& wo, const Vec3& wi) const = 0;
    virtual void sample(const Vec3& wo,
                        Sampler& sampler,
                        std::optional<PhaseFunctionSampleRecord>& pRec) const =
    0;
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
    void sample(const Vec3& wo,
                Sampler& sampler,
                std::optional<PhaseFunctionSampleRecord>& pRec) const override {
      Real u = sampler.sample();
      Real cosTheta{};
      if (std::abs(g) < 1e-6)
        cosTheta = 2 * g - 1;
      else {
        Real term = (1 - g * g) / (1 - g + 2 * g * u);
        cosTheta = (1 + g * g - term * term) / (2 * g);
      }
      ASSERT(cosTheta >= 0.0 && cosTheta <= 1.0,
             "sampled cosTheta out of range");
      Real sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
      if (sampler.sample() <= 0.5) sinTheta = -sinTheta;
      Real phi = 2 * PI * sampler.sample();
      pRec->wi = Vec3(sinTheta * std::cos(phi),
                     sinTheta * std::sin(phi),
                     cosTheta);
      pRec->pdf = u;
    }
    Real pdfSample(const Vec3& wo, const Vec3& wi) const override {
      Real cosTheta = dot(wo, wi);
      return (1 - g * g) / pow(1 + g * g - 2 * g * cosTheta, 1.5);
    }

  private:
    Real g;
};
}

#endif