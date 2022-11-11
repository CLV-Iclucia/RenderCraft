#include "Material.h"
#include "../XMath/ext/Graphics/MathUtils.h"
#include "../XMath/ext/Matrix.h"
#include <cmath>
/*Both wo and wi leaves the point*/
Vec3 Metal::Fresnel(Real cosTheta) const
{
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
Vec3 Metal::BxDF(const Vec3& wi, const Vec3& wo, const Vec2& uv) const
{
    if(wo[2] < 0.0) return {};
    Real cosThetaI = wi[2] + EPS;
    Real cosThetaO = wo[2] + EPS;
    Vec3 H = (wi + wo).normalize();
    Real cosThetaH = std::max(H[2], 0.0);
    Vec3 ret = 0.25 * Fresnel(std::min(H.dot(wo), 1.0)) * surface->NormalDistribution(cosThetaH)
        / cosThetaO * surface->ShadowMasking(cosThetaI, cosThetaO, uv) / cosThetaI;
    return ret;
}
Vec3 Metal::sample(const Vec3& wo, Real& pdf_inv, const Vec2& uv) const
{
    Vec3 H = surface->ImportanceSample(pdf_inv, uv);
    Real cosTheta = H.dot(wo);
    Vec3 ret = H * 2.0 * cosTheta - wo;
    while (cosTheta < 0.0 || ret[2] < 0.0)
    {
        H = surface->ImportanceSample(pdf_inv, uv);
        cosTheta = H.dot(wo);
        ret = H * 2.0 * cosTheta - wo;
    }
    pdf_inv *= 4.0 * cosTheta;
    return ret;
}
Vec3 Lambertian::sample(const Vec3& wo, Real& pdf_inv, const Vec2& uv) const
{
    if(wo[2] < 0.0) return {};
    Vec3 ret = cos_weighted_sample_hemisphere();
    pdf_inv = PI / std::max(ret[2], EPS);
    return ret;
}
Vec3 Lambertian::BxDF(const Vec3& wi, const Vec3& wo, const Vec2& uv) const
{
    if (wo[2] < 0.0) return {};
    else return albedo->eval(uv) * PI_INV;
}
Vec3 Translucent::sample(const Vec3& wo, Real& pdf_inv, const Vec2& uv) const
{
    bool inside = wo[2] < 0.0;
    Real eta = inside ? etaB / etaA : etaA / etaB; //eta = etaI / etaO
    Vec3 Wo = wo; // we flip the wo, it is just like we flip the z-axis
    if(inside) Wo[2] = -Wo[2];
    Vec3 H;
    Real etaSqr = eta * eta;
    Real cosTheta;
    Vec3 reflect_wi, refract_wi;
    if (1 - wo[2] * wo[2] > etaSqr)//sample reflection light only, else we can't compute Fresnel term
    {
        while (true)
        {
            H = surface->ImportanceSample(pdf_inv, uv);
            cosTheta = H.dot(Wo);
            reflect_wi = H * 2.0 * cosTheta - Wo;
            if (reflect_wi[2] < 0.0) continue;
            pdf_inv *= 4.0 * cosTheta;
            return reflect_wi;
        }
    }
    else
    {
        while (true) //keep generating samples until we produce valid samples
        {
            H = surface->ImportanceSample(pdf_inv, uv);
            cosTheta = H.dot(wo);
            reflect_wi = H * 2.0 * cosTheta - wo;
            if (1.0 - cosTheta * cosTheta > etaSqr)//a total reflection
            {
                if (reflect_wi[2] < 0.0) continue;
                pdf_inv *= 4.0 * cosTheta;
                return reflect_wi;
            }
            refract_wi = refract(wo, H, eta).normalize();
            if (reflect_wi[2] < 0.0)//in this case it is only possible to generate a refraction light
            {
                if (refract_wi[2] > 0.0) continue;
                else
                {
                    const Real cosThetaI = H.dot(refract_wi);
                    const Real tmp = (cosTheta + eta * cosThetaI);
                    pdf_inv *= tmp * tmp / (etaSqr * std::abs(cosThetaI));
                    return refract_wi;
                }
            }
            else if (refract_wi[2] > 0.0)//in this case it is only possible to generate a reflection light
            {
                pdf_inv *= 4.0 * cosTheta;
                return reflect_wi;
            }
            else
            {
                if (get_random() < 0.5)
                {
                    pdf_inv *= 8.0 * cosTheta;
                    return reflect_wi;
                }
                else
                {
                    const Real cosThetaI = H.dot(refract_wi);
                    const Real tmp = (cosTheta + eta * cosThetaI);
                    pdf_inv *= 2.0 * tmp * tmp / (etaSqr * std::abs(cosThetaI));
                    return refract_wi;
                }
            }
        }
    }
}
Real Translucent::Fresnel(Real cosThetaO, Real eta)
{
    const Real sinThetaO = std::sqrt(1.0 - cosThetaO * cosThetaO);
    const Real sinThetaI = sinThetaO / eta;
    const Real cosThetaI = std::sqrt(1.0 - sinThetaI * sinThetaI);
    Real tmp = eta * cosThetaO;
    const Real Rp = (cosThetaI - tmp) / (tmp + cosThetaI);
    tmp = eta * cosThetaI;
    const Real Rv = (tmp - cosThetaO) / (cosThetaO + tmp);
    return (Rv * Rv + Rp * Rp) * 0.5;
}
Vec3 Translucent::BxDF(const Vec3& wi, const Vec3& wo, const Vec2& uv) const
{
    bool inside = wo[2] < 0.0;
    Real eta = inside ? etaB / etaA : etaA / etaB; //eta = etaI / etaO
    Vec3 Wo = wo, Wi = wi;
    if(inside)
    {
        Wi[2] = -Wi[2];
        Wo[2] = -Wo[2];
    }
    Real cosThetaO = std::min(Wo[2], 1.0);
    Real cosThetaI = Wi[2];
    if (cosThetaI >= 0.0) //a reflection light
    {
        Vec3 H = (Wi + Wo).normalize();
        Real HdotO = std::min(H.dot(Wo), 1.0);
        Real Fr = 1.0 - HdotO * HdotO < eta * eta ? Fresnel(HdotO, eta) : 1.0;
        Real cosThetaH = std::max(H[2], 0.0);
        Vec3 ret = 0.25 * Fr * color->eval(uv) * surface->NormalDistribution(cosThetaH)
            / cosThetaO * surface->ShadowMasking(cosThetaI, cosThetaO, uv) / cosThetaI;
        return ret;
    }
    else //a refraction light
    {
        cosThetaI = -cosThetaI;
        Vec3 H = (Wi * eta + Wo).normalize();
        Real HdotO = std::min(H.dot(Wo), 1.0);
        Real Fr = 1.0 - HdotO * HdotO < eta * eta ? Fresnel(HdotO, eta) : 1.0;
        Real cosThetaH = std::abs(H[2]);
        Real HdotWi = H.dot(Wi);
        Real HdotWo = std::abs(H.dot(Wo));
        Real tmp = (HdotWo + eta * HdotWi);
        Vec3 ret = (1.0 - Fr) * std::abs(HdotWi) / cosThetaI * HdotWo / cosThetaO
            * surface->NormalDistribution(cosThetaH) * color->eval(uv) / (tmp * tmp)
            * surface->ShadowMasking(cosThetaI, cosThetaO, uv);
        return ret;
    }
}
