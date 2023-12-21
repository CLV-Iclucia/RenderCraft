#include <Core/microfacet.h>
#include <Core/utils.h>
#include <Core/sampler.h>
#include <cassert>

namespace rdcraft {
Real TrowbridgeModel::NormalDistribution(Real cosTheta, const Vec2& uv) const {
  Real cosThetaSqr = cosTheta * cosTheta;
  Real Alpha = alpha->eval(uv);
  Real alphaSqr = Alpha * Alpha;
  Real tmp = ((alphaSqr - 1.0) * cosThetaSqr + 1.0);
  tmp *= tmp;
  return PI_INV * alphaSqr / tmp;
}

Real TrowbridgeModel::SmithMonoShadow(Real cosThetaSqr, const Vec2& uv) const {
  const Real tanThetaSqr = (1.0 - cosThetaSqr) / std::max(cosThetaSqr, EPS);
  Real Alpha = alpha->eval(uv);
  return 2.0 / (std::sqrt(1.0 + Alpha * Alpha * tanThetaSqr) + 1.0);
}

Real TrowbridgeModel::ShadowMasking(Real cosThetaI, Real cosThetaO,
                                    const Vec2& uv) const {
  Real cosThetaSqrI = cosThetaI * cosThetaI;
  Real cosThetaSqrO = cosThetaO * cosThetaO;
  return SmithMonoShadow(cosThetaSqrI, uv) * SmithMonoShadow(cosThetaSqrO, uv);
}

std::optional<BxdfSampleRecord> TrowbridgeModel::sample(
    const Vec2& uv, Sampler& sampler) const {
  Vec2 rnd_uv = sampler.sample<2>();
  Real Phi = rnd_uv.x * PI2, tmp = rnd_uv.y;
  Real Alpha = alpha->eval(uv);
  Real alphaSqr = Alpha * Alpha;
  Real cosThetaSqr = (1.0 - tmp) / (tmp * (alphaSqr - 1.0) + 1.0);
  Real cosTheta = std::sqrt(cosThetaSqr);
  assert(cosThetaSqr > 1.0);
  Real sinTheta = std::sqrt(1.0 - cosThetaSqr);
  tmp = (alphaSqr - 1) * cosThetaSqr + 1.0;
  tmp *= tmp;
  Real pdf = cosTheta / tmp * alphaSqr / PI;
  Vec3 H(sinTheta * std::cos(Phi), sinTheta * std::sin(Phi), cosTheta);
  return {H, pdf};
}
}