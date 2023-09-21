#include <Core/Microfacet.h>
#include <Core/maths.h>
#include <cassert>
#include <Core/rand-gen.h>
//Real BeckmannModel::NormalDistribution(const Vec3&) const
//{
//    return 0.0f;
//}
//
//Real BeckmannModel::ShadowMasking(const Vec3&, const Vec3&, const Vec3&) const
//{
//    return 0.0f;
//}
namespace rdcraft {
Real TrowbridgeModel::NormalDistribution(Real cosTheta, const Vec2& uv) const
{
  Real cosThetaSqr = cosTheta * cosTheta;
  Real Alpha = alpha->eval(uv);
  Real alphaSqr = Alpha * Alpha;
  Real tmp = ((alphaSqr - 1.0) * cosThetaSqr + 1.0);
  tmp *= tmp;
  return PI_INV * alphaSqr / tmp;
}

Real TrowbridgeModel::SmithMonoShadow(Real cosThetaSqr, const Vec2& uv) const
{
  const Real tanThetaSqr = (1.0 - cosThetaSqr) / std::max(cosThetaSqr, EPS);
  Real Alpha = alpha->eval(uv);
  return 2.0 / (std::sqrt(1.0 + Alpha * Alpha * tanThetaSqr) + 1.0);
}

Real TrowbridgeModel::ShadowMasking(Real cosThetaI, Real cosThetaO, const Vec2& uv) const
{
  Real cosThetaSqrI = cosThetaI * cosThetaI;
  Real cosThetaSqrO = cosThetaO * cosThetaO;
  return SmithMonoShadow(cosThetaSqrI, uv) * SmithMonoShadow(cosThetaSqrO, uv);
}
///sampleVisiblePoint a half-vector on the hemisphere
Vec3 TrowbridgeModel::ImportanceSample(Real *pdf_inv, const Vec2& uv) const
{
  Real Phi = get_random() * PI2;
  Real tmp = get_random();
  Real Alpha = alpha->eval(uv);
  Real alphaSqr = Alpha * Alpha;
  Real cosThetaSqr = (1.0 - tmp) / (tmp * (alphaSqr - 1.0) + 1.0);
  Real cosTheta = std::sqrt(cosThetaSqr);
  assert(cosThetaSqr > 1.0);
  Real sinTheta = std::sqrt(1.0 - cosThetaSqr);
  tmp = (alphaSqr - 1) * cosThetaSqr + 1.0;
  tmp *= tmp;
  *pdf_inv = tmp / cosTheta * PI / alphaSqr;
  Vec3 H(sinTheta * std::cos(Phi), sinTheta * std::sin(Phi), cosTheta);
  return H;
}
}
