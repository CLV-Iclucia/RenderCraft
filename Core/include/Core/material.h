#ifndef RENDERCRAFT_MATERIAL_H
#define RENDERCRAFT_MATERIAL_H
#include <Core/microfacet.h>
#include <Core/spectrums.h>
#include <Core/core.h>
#include <memory>
#include <optional>
#include <utility>

namespace rdcraft {
struct SurfaceInteraction;
struct ShadingInfo;
class Sampler;

struct Material {
  virtual std::optional<Real> getTransmissive() const {
    return std::nullopt;
  }
  virtual Spectrum computeScatter(const ShadingInfo& si) const = 0;
  virtual std::optional<BxdfSampleRecord> sample(
      const ShadingInfo& si, Sampler& sampler) const = 0;
  virtual Real pdfSample(const ShadingInfo& si, const Vec3& wi) const = 0;
  virtual ~Material();
};

class Lambertian final : public Material {
  // Lambertian diffuse
  public:
    explicit Lambertian(std::unique_ptr<SurfaceTexture<Spectrum>> _albedo)
      : albedo(std::move(_albedo)) {
    }
    explicit Lambertian(const Spectrum& col)
      : albedo(std::make_unique<ConstantTexture<Spectrum, 2>>(col)) {
    }
    std::optional<BxdfSampleRecord> sample(const ShadingInfo& si,
                                           Sampler& sampler) const override;
    Spectrum computeScatter(const ShadingInfo& si) const override;
    Real pdfSample(const ShadingInfo& si, const Vec3& wi) const override;
  private:
    std::unique_ptr<SurfaceTexture<Spectrum>> albedo{};
};

class Metal final : public Material {
  // using Torrance-Sparrow Model
  public:
    Metal(const Vec3& _eta, const Vec3& _k, Microfacet* surf)
      : bxdf(surf), eta(_eta), k(_k) {
    }
    Spectrum computeScatter(const ShadingInfo& si) const override;
    std::optional<BxdfSampleRecord> sample(const ShadingInfo& si,
                                           Sampler& sampler) const override;
    Real pdfSample(const ShadingInfo& si, const Vec3& wi) const override;
  private:
    Vec3 Fresnel(Real) const;
    std::unique_ptr<Microfacet> bxdf{};
    Vec3 eta, k; // /bar{eta} = eta + ik according to PBR
};

// note: this describes a interface instead of a material
// which means that the refraction rate is eta1 / eta2
class Dieletrics final : public Material {
  // translucent dielectrics
  public:
    // follows PBRT-V4 to handle Russian Roulette efficiently
    std::optional<Real> getTransmissive() const override {
      return std::make_optional(refraction_rate);
    }
    Dieletrics(const Real _eta, std::unique_ptr<Microfacet> _bxdf)
      : bxdf(std::move(_bxdf)), refraction_rate(_eta) {
    }
    std::optional<BxdfSampleRecord> sample(const ShadingInfo& si,
                                           Sampler& sampler) const override;
    Spectrum computeScatter(const ShadingInfo& si) const override;
    Real pdfSample(const ShadingInfo& si, const Vec3& wi) const override;
  protected:
    std::unique_ptr<SurfaceTexture<Spectrum>> color;
    std::unique_ptr<Microfacet> bxdf = nullptr;
    Real refraction_rate{1.0}; // defined to be eta_in / eta_out
};
} // namespace rdcraft
#endif