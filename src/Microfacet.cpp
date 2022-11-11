#include "Microfacet.h"
#include "../XMath/ext/Graphics/MathUtils.h"
#include "../XMath/ext/Matrix.h"
//Real BeckmannModel::NormalDistribution(const Vec3&) const
//{
//    return 0.0f;
//}
//
//Real BeckmannModel::ShadowMasking(const Vec3&, const Vec3&, const Vec3&) const
//{
//    return 0.0f;
//}

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
///sample a half-vector on the hemisphere
Vec3 TrowbridgeModel::ImportanceSample(Real& pdf_inv, const Vec2& uv) const
{
    Real Phi = get_random() * PI2;
    Real tmp = get_random();
    try
    {
        Real Alpha = alpha->eval(uv);
        Real alphaSqr = Alpha * Alpha;
        Real cosThetaSqr = (1.0 - tmp) / (tmp * (alphaSqr - 1.0) + 1.0);
        Real cosTheta = std::sqrt(cosThetaSqr);
        if(cosThetaSqr > 1.0) throw std::runtime_error("ERROR: cosine greater than 1 "
                                                   "in TrowbridgeModel::ImportanceSample");
        Real sinTheta = std::sqrt(1.0 - cosThetaSqr);
        tmp = (alphaSqr - 1) * cosThetaSqr + 1.0;
        tmp *= tmp;
        pdf_inv = tmp / cosTheta * PI / alphaSqr;
        Vec3 H({ sinTheta * std::cos(Phi), sinTheta * std::sin(Phi), cosTheta });
        return H;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

Vec3 TrowbridgeModel::SampleVNDF(Real& pdf_inv) const
{

}

Real NormalMapMicrofacet::NormalDistribution(Real, const Vec2 &) const
{
    return 0;
}
