#include <Core/material.h>
#include <Core/utils.h>
#include <Core/interaction.h>
#include <Core/sampler.h>
#include <cmath>

namespace rdcraft {
/*Both wo and wi leaves the point*/
Vec3 Metal::Fresnel(Real cosTheta) const {
  const Real cosThetaSqr = cosTheta * cosTheta;
  const Real sinThetaSqr = 1.0 - cosThetaSqr;
  Vec3 etaSqr = eta * eta;
  const Vec3 VecSinThetaSqr(sinThetaSqr);
  const Vec3 VecCosThetaSqr(cosThetaSqr);
  Vec3 kSqr = k * k;
  Vec3 ASqrPlusBSqr = etaSqr - kSqr - VecSinThetaSqr;
  ASqrPlusBSqr *= ASqrPlusBSqr;
  ASqrPlusBSqr += 4.0 * etaSqr * kSqr;
  for (int i = 0; i < 3; i++)
    ASqrPlusBSqr[i] = std::sqrt(ASqrPlusBSqr[i]);
  const Vec3 ASqr = (ASqrPlusBSqr + etaSqr - kSqr - VecSinThetaSqr) * 0.5;
  Vec3 A;
  for (int i = 0; i < 3; i++)
    A[i] = std::sqrt(ASqr[i]);
  Vec3 tmp = ASqrPlusBSqr + VecCosThetaSqr;
  Vec3 Tmp = A * 2.0 * cosTheta;
  const Vec3 Rv = (tmp - Tmp) / (tmp + Tmp);
  tmp = ASqrPlusBSqr * cosThetaSqr + VecSinThetaSqr * sinThetaSqr;
  Tmp *= sinThetaSqr;
  const Vec3 Rp = Rv * (tmp - Tmp) / (tmp + Tmp);
  return (Rv * Rv + Rp * Rp) * 0.5;
}

Spectrum Metal::computeScatter(const ShadingInfo& si) const {
  const auto& wo = si.wo;
  const auto& wi = si.wi;
  if (wo.z < 0.0)
    return {};
  Real cosThetaI {wi.z};
  Real cosThetaO {wo.z};
  Vec3 H = normalize(wi + wo);
  Real cosThetaH = std::max(H[2], 0.0);
  Spectrum ret = 0.25 * Fresnel(std::min(dot(H, wo), 1.0)) *
             bxdf->NormalDistribution(cosThetaH, si.uv) / cosThetaO *
             bxdf->ShadowMasking(cosThetaI, cosThetaO, si.uv) / cosThetaI;
  return ret;
}

std::optional<BxdfSampleRecord> Metal::sample(const ShadingInfo& si,
                                              Sampler& sampler) const {
  const auto& wo = si.wo;
  auto bRec = bxdf->sample(si.uv, sampler);
  if (!bRec) return std::nullopt;
  const auto& H = bRec->wi;
  Real cosTheta{dot(H, wo)};
  Vec3 ret = H * 2.0 * cosTheta - wo;
  if (cosTheta < 0.0 || ret.z < 0.0)
    return std::nullopt;
  bRec->pdf *= 4.0 * cosTheta;
  return std::make_optional<BxdfSampleRecord>(ret, bRec->pdf);
}

std::optional<BxdfSampleRecord> Lambertian::sample(
    const ShadingInfo& si, Sampler& sampler) const {
  if (si.wo.z < 0.0)
    return std::nullopt;
  Vec3 ret = cosWeightedSampleHemisphere();
  Real pdf = std::max(ret.z, 0.0) / PI;
  return std::make_optional<BxdfSampleRecord>(ret, pdf);
}

Spectrum Lambertian::computeScatter(const ShadingInfo& si) const {
  if (si.wo.z < 0.0)
    return {};
  else
    return albedo->eval(si.uv) * PI_INV;
}

static Real Fresnel(Real cosThetaO, Real eta) {
  Real sinThetaO = std::sqrt(1.0 - cosThetaO * cosThetaO);
  assert(cosThetaO <= 1.0);
  Real sinThetaI = sinThetaO / eta;
  assert(sinThetaI <= 1.0);
  Real cosThetaI = std::sqrt(1.0 - sinThetaI * sinThetaI);
  Real tmp = eta * cosThetaO;
  Real Rp = (cosThetaI - tmp) / (tmp + cosThetaI);
  tmp = eta * cosThetaI;
  Real Rv = (tmp - cosThetaO) / (cosThetaO + tmp);
  return (Rv * Rv + Rp * Rp) * 0.5;
}

std::optional<BxdfSampleRecord> Dieletrics::sample(
    const ShadingInfo& si, Sampler& sampler) const {
  const auto& wo = si.wo;
  bool inside = wo.z < 0.0;
  // Real eta = inside ? interface.etaB / etaA : etaA / etaB; // eta = etaI / etaO
  Real eta = 1.0;
  Vec3 Wo = wo; // we flip the wo, it is just like we flip the z-axis
  if (inside)
    Wo.z = -Wo.z;
  auto optRec = bxdf->sample(si.uv, sampler);
  if (!optRec) return std::nullopt;
  Vec3 H{optRec->wi};
  Real pdf{optRec->pdf};
  Real etaSqr = eta * eta;
  Real cosTheta = dot(H, Wo);
  Real Fr = Fresnel(cosTheta, eta);
  if (sampler.get() < Fr) {
    Vec3 reflect_wi = H * 2.0 * cosTheta - Wo;
    if (reflect_wi.z < 0.0)
      return std::nullopt;
    pdf /= 4.0 * cosTheta;
    return std::make_optional<BxdfSampleRecord>(reflect_wi, pdf);
  }
  Vec3 refract_wi = normalize(refract(Wo, H, eta));
  if (refract_wi.z > 0.0)
    // in this case it is only possible to generate a reflection light
      return std::nullopt;
  Real cosThetaI = dot(H, refract_wi);
  Real tmp = cosTheta + eta * cosThetaI;
  pdf /= 2.0 * tmp * tmp / (etaSqr * std::abs(cosThetaI));
  if (inside)
    refract_wi.z = -refract_wi.z;
  return std::make_optional<BxdfSampleRecord>(refract_wi, pdf);
}

Spectrum Dieletrics::computeScatter(const ShadingInfo& si) const {
  bool inside = si.wo.z < 0.0;
  // TODO: fix this
  // Real eta = inside ? etaB / etaA : etaA / etaB; // eta = etaI / etaO
  Real eta = 1.0;
  Vec3 Wo{si.wo}, Wi{si.wi};
  if (inside) {
    Wi.z = -Wi.z;
    Wo.z = -Wo.z;
  }
  Real cosThetaO = std::min(Wo[2], 1.0);
  Real cosThetaI = Wi[2];
  Vec3 ret;
  if (cosThetaI >= 0.0) {
    // a reflection light
    Vec3 H = normalize(Wi + Wo);
    Real HdotO = std::min(dot(H, Wo), 1.0);
    Real Fr = 1.0 - HdotO * HdotO < eta * eta ? Fresnel(HdotO, eta) : 1.0;
    Real cosThetaH = std::max(H[2], 0.0);
    ret = 0.25 * Fr * color->eval(si.uv) *
          bxdf->NormalDistribution(cosThetaH, si.uv) / cosThetaO *
          bxdf->ShadowMasking(cosThetaI, cosThetaO, si.uv) / cosThetaI;
  } else {
    // a refraction light
    cosThetaI = -cosThetaI;
    Vec3 H = normalize(Wi * eta + Wo);
    Real HdotO = std::min(dot(H, Wo), 1.0);
    Real Fr = 1.0 - HdotO * HdotO < eta * eta ? Fresnel(HdotO, eta) : 1.0;
    Real cosThetaH = std::abs(H[2]);
    Real HdotWi = dot(H, Wi);
    Real HdotWo = std::abs(dot(H, Wo));
    Real tmp = (HdotWo + eta * HdotWi);
    ret = (1.0 - Fr) * std::abs(HdotWi) / cosThetaI * HdotWo / cosThetaO *
          bxdf->NormalDistribution(cosThetaH, si.uv) * color->eval(si.uv) /
          (tmp * tmp) * bxdf->ShadowMasking(cosThetaI, cosThetaO, si.uv);
  }
  return ret;
}
} // namespace rdcraft