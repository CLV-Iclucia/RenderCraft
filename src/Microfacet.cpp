#include "Microfacet.h"
#include "../XMath/MyMath.h"
#include "Matrix.h"
//Real BeckmannModel::NormalDistribution(const Vec3&) const
//{
//    return 0.0f;
//}
//
//Real BeckmannModel::ShadowMasking(const Vec3&, const Vec3&, const Vec3&) const
//{
//    return 0.0f;
//}

Real TrowbridgeModel::NormalDistribution(Real cosTheta) const
{
    const Real cosThetaSqr = cosTheta * cosTheta;
    const Real alphaSqr = alpha * alpha;
    Real tmp = ((alphaSqr - 1.0) * cosThetaSqr + 1.0);
    tmp *= tmp;
    return PI_INV * alphaSqr / tmp;
}

Real TrowbridgeModel::SmithMonoShadow(Real cosThetaSqr) const
{
    const Real tanThetaSqr = (1.0 - cosThetaSqr) / std::max(cosThetaSqr, EPS);
    return 2.0 / (std::sqrt(1.0 + alpha * alpha * tanThetaSqr) + 1.0);
}

Real TrowbridgeModel::ShadowMasking(Real cosThetaI, Real cosThetaO) const
{
    Real cosThetaSqrI = cosThetaI * cosThetaI;
    Real cosThetaSqrO = cosThetaO * cosThetaO;
    return SmithMonoShadow(cosThetaSqrI) * SmithMonoShadow(cosThetaSqrO);
}

Vec3 TrowbridgeModel::ImportanceSample(Real& pdf_inv) const //sample a half-vector on the hemisphere
{
    const Real Phi = get_random() * PI2;
    Real tmp = get_random();
    const Real alphaSqr = alpha * alpha;
    const Real cosThetaSqr = (1.0 - tmp) / (tmp * (alphaSqr - 1.0) + 1.0);
    Real cosTheta = std::sqrt(cosThetaSqr);
    const Real sinTheta = std::sqrt(1.0 - cosThetaSqr);
    tmp = (alphaSqr - 1) * cosThetaSqr + 1.0;
    tmp *= tmp;
    pdf_inv = tmp / cosTheta * PI / alphaSqr;
    Vec3 H({ sinTheta * std::cos(Phi), sinTheta * std::sin(Phi), cosTheta });
    return H;
}