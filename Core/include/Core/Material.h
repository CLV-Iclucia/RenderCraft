#ifndef RENDERCRAFT_MATERIAL_H
#define RENDERCRAFT_MATERIAL_H
#include <Core/medium.h>
#include <Core/Microfacet.h>
#include <spectrums.h>
#include <Core/core.h>
#include <memory>
#include <utility>

namespace rdcraft {
struct SurfaceRecord;
struct Material {
  virtual Vec3 BxDF(const SurfaceRecord &, const Vec3 &wo, const Vec3 &wi) const = 0;
  virtual Vec3 sample(const SurfaceRecord &, const Vec3 &wo, Real *pdf) const = 0;
};
class Lambertian : public Material {
  // Lambertian diffuse
public:
  explicit Lambertian(std::unique_ptr<Texture<Spectrum>> _albedo)
      : albedo(std::move(_albedo)) {}
  explicit Lambertian(const Spectrum &col)
      : albedo(std::make_unique<ConstantTexture<Spectrum>>(col)) {}
  Vec3 sample(const SurfaceRecord &, const Vec3 &, Real *) const override;
  Vec3 BxDF(const SurfaceRecord &, const Vec3 &, const Vec3 &) const override;

private:
  std::unique_ptr<Texture<Spectrum>> albedo;
};
class Metal : public Material {
  // using Torrance-Sparrow Model
public:
  Metal(Real etaR, Real etaG, Real etaB, Real kR, Real kG, Real kB,
        Microfacet *surf)
      : eta({etaR, etaG, etaB}), k({kR, kG, kB}), surface(surf) {}
  Metal(Vec3 _eta, Vec3 _k, Microfacet *surf)
      : eta(std::move(_eta)), k(std::move(_k)), surface(surf) {}
  Vec3 BxDF(const SurfaceRecord &, const Vec3 &, const Vec3 &) const override;
  Vec3 sample(const SurfaceRecord &, const Vec3 &, Real *) const override;

private:
  Vec3 Fresnel(Real) const;
  std::unique_ptr<Microfacet> surface =
      nullptr; // surface = nullptr means that the surface is smooth
  Vec3 eta, k; // /bar{eta} = eta + ik according to PBR
};

class Dieletrics : public Material {
  // translucent dielectrics
public:
  Vec3 sample(const SurfaceRecord &, const Vec3 &, Real *) const override;
  Vec3 BxDF(const SurfaceRecord &, const Vec3 &, const Vec3 &) const override;

protected:
  std::unique_ptr<Texture<Spectrum>> color;
  std::unique_ptr<Microfacet> surface = nullptr;
  // TODO: I think this should be handled with care
  Real refraction_rate = 1.0;
};
} // namespace rdcraft
#endif