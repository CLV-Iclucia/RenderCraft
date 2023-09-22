#ifndef RENDERCRAFT_MATERIAL_H
#define RENDERCRAFT_MATERIAL_H
#include "Core/core.h"
#include "Medium.h"
#include <Core/Microfacet.h>
#include <Core/Spectrums.h>
#include <memory>
#include <utility>

namespace rdcraft {
struct Material {
  virtual Vec3 BxDF(const Vec3 &, const Vec3 &, const Vec2 &) const = 0;
  virtual Vec3 sample(const Vec3 &, Real *, const Vec2 &) const = 0;
};
class Lambertian : public Material {
  // Lambertian diffuse
public:
  explicit Lambertian(std::shared_ptr<Texture<Spectrum>> _albedo)
      : albedo(std::move(_albedo)) {}
  explicit Lambertian(const Spectrum &col)
      : albedo(std::make_shared<ConstantTexture<Spectrum>>(col)) {}
  Vec3 sample(const Vec3 &, Real *, const Vec2 &) const override;
  Vec3 BxDF(const Vec3 &, const Vec3 &, const Vec2 &) const override;

private:
  std::shared_ptr<Texture<Spectrum>> albedo;
};
class Metal : public Material {
  // using Torrance-Sparrow Model
public:
  Metal(Real etaR, Real etaG, Real etaB, Real kR, Real kG, Real kB,
        Microfacet *surf)
      : eta({etaR, etaG, etaB}), k({kR, kG, kB}), surface(surf) {}
  Metal(Vec3 _eta, Vec3 _k, Microfacet *surf)
      : eta(std::move(_eta)), k(std::move(_k)), surface(surf) {}
  Vec3 BxDF(const Vec3 &, const Vec3 &, const Vec2 &) const override;
  Vec3 sample(const Vec3 &, Real *, const Vec2 &) const override;

private:
  Vec3 Fresnel(Real) const;
  Microfacet *surface = nullptr; // surface = nullptr means that the surface is smooth
  Vec3 eta, k; // /bar{eta} = eta + ik according to PBR
};

// this class in fact describe a surface between two translucent media
class Dieletrics : public Material {
  // translucent dielectrics
public:
  Vec3 sample(const Vec3 &, Real *, const Vec2 &) const override;
  Vec3 BxDF(const Vec3 &wi, const Vec3 &wo, const Vec2 &uv) const override;

private:
  std::shared_ptr<Texture<Spectrum>> color;
  Microfacet *surface = nullptr;
  // TODO: I think this should be handled with care
  Real refraction_rate = 1.0;
};
} // namespace rdcraft
#endif