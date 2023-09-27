#include <Core/Material.h>
#include <Core/maths.h>
#include <cmath>
#include <Core/Record.h>
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
Vec3 Metal::BxDF(const SurfaceRecord& rec, const Vec3& wo, const Vec3& wi) const {
  if (wo[2] < 0.0)
    return {};
  Real cosThetaI = wi[2] + EPS;
  Real cosThetaO = wo[2] + EPS;
  Vec3 H = normalize(wi + wo);
  Real cosThetaH = std::max(H[2], 0.0);
  Vec3 ret = 0.25 * Fresnel(std::min(dot(H, wo), 1.0)) *
             surface->NormalDistribution(cosThetaH, rec.uv) / cosThetaO *
             surface->ShadowMasking(cosThetaI, cosThetaO, rec.uv) / cosThetaI;
  return ret;
}
Vec3 Metal::sample(const SurfaceRecord &rec, const Vec3& wo, Real *pdf) const {
  Vec3 H = surface->ImportanceSample(pdf, rec.uv);
  Real cosTheta = dot(H, wo);
  Vec3 ret = H * 2.0 * cosTheta - wo;
  if (cosTheta < 0.0 || ret[2] < 0.0) {
    *pdf = 0.0;
    return {};
  }
  *pdf *= 4.0 * cosTheta;
  return ret;
}
Vec3 Lambertian::sample(const SurfaceRecord &rec, const Vec3& wo, Real *pdf) const {
  if (wo.z < 0.0)
    return {};
  Vec3 ret = cosWeightedSampleHemisphere();
  *pdf = std::max(ret.z, 0.0) / PI;
  return ret;
}
Vec3 Lambertian::BxDF(const SurfaceRecord& rec, const Vec3& wo, const Vec3& wi) const {
  if (wo.z < 0.0)
    return {};
  else
    return albedo->eval(rec.uv) * PI_INV;
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
Vec3 Dieletrics::sample(const SurfaceRecord& rec, const Vec3 &wo, Real *pdf) const {
  bool inside = wo.z < 0.0;
  // Real eta = inside ? interface.etaB / etaA : etaA / etaB; // eta = etaI / etaO
  Real eta = 1.0;
  Vec3 Wo = wo; // we flip the wo, it is just like we flip the z-axis
  if (inside)
    Wo.z = -Wo.z;
  Vec3 H = surface->ImportanceSample(pdf, rec.uv);
  Real etaSqr = eta * eta;
  Real cosTheta = dot(H, Wo);
  Real Fr = Fresnel(cosTheta, eta);
  if (randomReal() < Fr) {
    Vec3 reflect_wi = H * 2.0 * cosTheta - Wo;
    if (reflect_wi.z < 0.0) {
      *pdf = 0.0;
      return {};
    }
    *pdf /= 4.0 * cosTheta;
    return reflect_wi;
  } else {
    Vec3 refract_wi = normalize(refract(Wo, H, eta));
    if (refract_wi[2] > 0.0) {
      // in this case it is only possible to generate a reflection light
      *pdf = 0.0;
      return {};
    } else {
      Real cosThetaI = dot(H, refract_wi);
      Real tmp = cosTheta + eta * cosThetaI;
      *pdf /= 2.0 * tmp * tmp / (etaSqr * std::abs(cosThetaI));
      if (inside)
        refract_wi[2] = -refract_wi[2];
      return refract_wi;
    }
  }
}

Vec3 Dieletrics::BxDF(const SurfaceRecord& rec, const Vec3 &wo, const Vec3 &wi) const {
  bool inside = wo.z < 0.0;
  // TODO: fix this
 // Real eta = inside ? etaB / etaA : etaA / etaB; // eta = etaI / etaO
  Real eta = 1.0;
  Vec3 Wo = wo, Wi = wi;
  if (inside) {
    Wi.z = -Wi.z;
    Wo.z = -Wo.z;
  }
  Real cosThetaO = std::min(Wo[2], 1.0);
  Real cosThetaI = Wi[2];
  if (cosThetaI >= 0.0) {
    // a reflection light
    Vec3 H = normalize(Wi + Wo);
    Real HdotO = std::min(dot(H, Wo), 1.0);
    Real Fr = 1.0 - HdotO * HdotO < eta * eta ? Fresnel(HdotO, eta) : 1.0;
    Real cosThetaH = std::max(H[2], 0.0);
    Vec3 ret = 0.25 * Fr * color->eval(rec.uv) *
               surface->NormalDistribution(cosThetaH, rec.uv) / cosThetaO *
               surface->ShadowMasking(cosThetaI, cosThetaO, rec.uv) / cosThetaI;
    return ret;
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
    Vec3 ret = (1.0 - Fr) * std::abs(HdotWi) / cosThetaI * HdotWo / cosThetaO *
               surface->NormalDistribution(cosThetaH, rec.uv) * color->eval(rec.uv) /
               (tmp * tmp) * surface->ShadowMasking(cosThetaI, cosThetaO, rec.uv);
    return ret;
  }
}
} // namespace rdcraft