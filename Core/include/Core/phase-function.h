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
    virtual Spectrum eval(const Vec3& wo, const Vec3& wi) const = 0;
    virtual void sample(const Vec3& wo,
                        Sampler& sampler,
                        std::optional<PhaseFunctionSampleRecord>& pRec) const =
    0;
    virtual Real pdfSample(const Vec3& wo, const Vec3& wi) const = 0;
    virtual ~PhaseFunction() = default;
};

class HenyeyGreenstein final : public PhaseFunction {
  public:
    explicit HenyeyGreenstein(Real _g)
      : g(_g) {
    }
    Spectrum eval(const Vec3& wo, const Vec3& wi) const override {
      Real cosTheta = dot(wo, wi);
      Real term = 1.0 + g * g + 2.0 * g * cosTheta;
      return Spectrum(
          (1.0 - g * g) / (PI4 * term * std::sqrt(term)));
    }
    void sample(const Vec3& wo,
                Sampler& sampler,
                std::optional<PhaseFunctionSampleRecord>& pRec) const override {
      Real u = sampler.sample();
      Real cosTheta{};
      if (std::abs(g) < 1e-6)
        cosTheta = 1 - 2 * g;
      else {
        Real term = (1 - g * g) / (1 - g + 2 * g * u);
        cosTheta = (1 + g * g - term * term) / (2 * g);
      }
      ASSERT(cosTheta >= -1.0 && cosTheta <= 1.0,
             "sampled cosTheta out of range");
      Real sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
      if (sampler.sample() <= 0.5) sinTheta = -sinTheta;
      Real phi = 2 * PI * sampler.sample();
      Mat3 TBN = constructFrame(wo);
      pRec.emplace(TBN * Vec3(sinTheta * std::cos(phi),
                              sinTheta * std::sin(phi),
                              cosTheta),
                   (1.0 - g * g) / (PI4 * u * std::sqrt(u)));
    }
    Real pdfSample(const Vec3& wo, const Vec3& wi) const override {
      Real cosTheta = dot(wo, wi);
      Real term = 1.0 + g * g + 2.0 * g * cosTheta;
      return (1.0 - g * g) / (PI4 * term * std::sqrt(term));
    }

  private:
    Real g;
};

class IsotropicPhaseFunction final : public PhaseFunction {
  public:
    Spectrum eval(const Vec3& wo, const Vec3& wi) const override {
      return Spectrum(PI4_INV);
    }
    void sample(const Vec3& wo,
                Sampler& sampler,
                std::optional<PhaseFunctionSampleRecord>& pRec) const override {
      Real u = sampler.sample();
      Real cosTheta = 1.0 - 2.0 * u;
      Real sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
      Real phi = 2 * PI * sampler.sample();
      Mat3 TBN = constructFrame(wo);
      pRec.emplace(TBN * Vec3(sinTheta * std::cos(phi),
                              sinTheta * std::sin(phi),
                              cosTheta), PI4_INV);
    }
    Real pdfSample(const Vec3& wo, const Vec3& wi) const override {
      return PI4_INV;
    }
};
}

#endif