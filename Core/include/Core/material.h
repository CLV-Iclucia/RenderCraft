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
  virtual Spectrum computeScatter(const ShadingInfo& si) const = 0;
  virtual std::optional<BxdfSampleRecord> sample(
      const ShadingInfo& si, Sampler& sampler) const = 0;
  virtual ~Material();
};

class Lambertian : public Material {
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

  private:
    std::unique_ptr<SurfaceTexture<Spectrum>> albedo{};
};

class Metal : public Material {
  // using Torrance-Sparrow Model
  public:
    Metal(const Vec3& _eta, const Vec3& _k, Microfacet* surf)
      : eta(_eta), k(_k), bxdf(surf) {
    }
    Spectrum computeScatter(const ShadingInfo& si) const override;
    std::optional<BxdfSampleRecord> sample(const ShadingInfo& si,
                                           Sampler& sampler) const override;

  private:
    Vec3 Fresnel(Real) const;
    std::unique_ptr<Microfacet> bxdf{};
    // surface = nullptr means that the surface is smooth
    Vec3 eta, k; // /bar{eta} = eta + ik according to PBR
};

class Dieletrics : public Material {
  // translucent dielectrics
  public:
    std::optional<BxdfSampleRecord> sample(const ShadingInfo& si,
                                           Sampler& sampler) const override;
    Spectrum computeScatter(const ShadingInfo& si) const override;

  protected:
    std::unique_ptr<SurfaceTexture<Spectrum>> color;
    std::unique_ptr<Microfacet> bxdf = nullptr;
    // TODO: I think this should be handled with care
    Real refraction_rate{1.0};
};
} // namespace rdcraft
#endif