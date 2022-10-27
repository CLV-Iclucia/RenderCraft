#include "Material.h"
#include "../XMath/MyMath.h"
#include "Matrix.h"
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
Vec3 Metal::BxDF(const Vec3& wi, const Vec3& wo, const Vec3& N) const
{
    Real cosThetaI = wi.dot(N) + EPS;
    Real cosThetaO = wo.dot(N) + EPS;
    const Vec3 H = (wi + wo).normalize();
    const Real cosThetaH = std::max(N.dot(H), 0.0);
    Vec3 ret = 0.25 * Fresnel(cosThetaO) * surface->NormalDistribution(cosThetaH)
        / cosThetaO * surface->ShadowMasking(cosThetaI, cosThetaO) / cosThetaI;
    return ret;
}
Vec3 Metal::sample(const Vec3& normal, const Vec3& wo, Real& pdf_inv) const
{
    Vec3 IS = surface->ImportanceSample(pdf_inv);
    Vec3 T({ 1.0, 0.0, 0.0 });
    if (normal[0] >= 0.99 || normal[0] <= -0.99)
    {
        T[0] = 0.0;
        T[1] = 1.0;
    }
    const Vec3 B = cross(normal, T).normalize();
    T = cross(normal, B);
    const Mat3f TBN(T, B, normal);
    Vec3 H = TBN * IS;
    Real cosTheta = H.dot(wo);
    Vec3 ret = H * 2.0 * cosTheta - wo;
    while (cosTheta < 0.0 || normal.dot(ret) < 0.0)
    {
        IS = surface->ImportanceSample(pdf_inv);
        H = TBN * IS;
        cosTheta = H.dot(wo);
        ret = H * 2.0 * cosTheta - wo;
    }
    pdf_inv *= 4.0 * cosTheta;
    return ret;
}
Vec3 Metal::eval(const Vec3& wi, const Vec3& wo, const Vec3& N) const
{
    const Real cosThetaI = N.dot(wi) + EPS;
    const Real cosThetaO = N.dot(wo) + EPS;
    const Vec3 H = (wi + wo).normalize();
    const Vec3 Fr = Fresnel(cosThetaO);
    Vec3 ret = Fr / cosThetaO *
        surface->ShadowMasking(cosThetaI, cosThetaO) * H.dot(wi) / N.dot(H);
    return ret;
}
Vec3 Lambertian::eval(const Vec3&, const Vec3&, const Vec3&) const
{
    return albedo * PI_INV;
}
Vec3 Lambertian::sample(const Vec3& normal, const Vec3&, Real& pdf_inv) const
{
    Vec3 ret = cos_weighted_sample_hemisphere();
    pdf_inv = PI / std::max(ret[2], EPS);
    Vec3 T({ 1.0, 0.0, 0.0 });
    if (normal[0] >= 0.99 || normal[0] <= -0.99)
    {
        T[0] = 0.0;
        T[1] = 1.0;
    }
    const Vec3 B = cross(normal, T).normalize();
    T = cross(normal, B);
    const Mat3f TBN(T, B, normal);
    return TBN * ret;
}
Vec3 Lambertian::BxDF(const Vec3& wi, const Vec3& wo, const Vec3& N) const
{
    if (wo.dot(N) < 0.0) return {};
    else return albedo * PI_INV;
}
Vec3 Translucent::sample(const Vec3& N, const Vec3& wo, Real& pdf_inv) const
{
    Vec3 normal;
    bool inside = checkInside(wo, N);
    if (inside) normal = N * (-1.0);
    else normal = N;
    const Real eta = inside ? etaB / etaA : etaA / etaB; //eta = etaI / etaO
    Vec3 T({ 1.0, 0.0, 0.0 });
    if (normal[0] >= 0.99 || normal[0] <= -0.99)
    {
        T[0] = 0.0;
        T[1] = 1.0;
    }
    const Vec3 B = cross(normal, T).normalize();
    T = cross(normal, B);
    const Mat3f TBN(T, B, normal);
    Vec3 H, IS;
    const Real etaSqr = eta * eta;
    Real cosThetaO = normal.dot(wo);
    Real cosTheta;
    Vec3 reflect_wi, refract_wi;
    if (1 - cosThetaO * cosThetaO > etaSqr)//sample reflection light only, else we can't compute Fresnel term
    {
        while (true)
        {
            IS = surface->ImportanceSample(pdf_inv);
            H = TBN * IS;
            cosTheta = H.dot(wo);
            reflect_wi = H * 2.0 * cosTheta - wo;
            if (normal.dot(reflect_wi) < 0.0) continue;
            pdf_inv *= 4.0 * cosTheta;
            return reflect_wi;
        }
    }
    else
    {
        while (true) //keep generating samples until we produce valid samples
        {
            IS = surface->ImportanceSample(pdf_inv);
            H = TBN * IS;
            cosTheta = H.dot(wo);
            reflect_wi = H * 2.0 * cosTheta - wo;
            if (1.0 - cosTheta * cosTheta > etaSqr)//a total reflection
            {
                if (normal.dot(reflect_wi) < 0.0) continue;
                pdf_inv *= 4.0 * cosTheta;
                return reflect_wi;
            }
            refract_wi = refract(wo, H, eta).normalize();
            if (normal.dot(reflect_wi) < 0.0)//in this case it is only possible to generate a refraction light
            {
                if (normal.dot(refract_wi) > 0.0) continue;
                else
                {
                    const Real cosThetaI = H.dot(refract_wi);
                    const Real tmp = (cosTheta + eta * cosThetaI);
                    pdf_inv *= tmp * tmp / (etaSqr * std::abs(cosThetaI));
                    return refract_wi;
                }
            }
            else if (normal.dot(refract_wi) > 0.0)//in this case it is only possible to generate a reflection light
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
Vec3 Translucent::eval(const Vec3& wi, const Vec3& wo, const Vec3& N) const
{
    return Vec3(0.0);
}
Real Translucent::Fresnel(Real cosThetaO, Real eta) const
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
Vec3 Translucent::BxDF(const Vec3& wi, const Vec3& wo, const Vec3& N) const
{
    Vec3 normal;
    bool inside = checkInside(wo, N);
    if (inside) normal = N * (-1.0);
    else normal = N;
    const Real eta = inside ? etaB / etaA : etaA / etaB; //eta = etaI / etaO
    const Real cosThetaO = std::min(normal.dot(wo), 1.0);
    Real cosThetaI = normal.dot(wi);
    const Real Fr = 1.0 - cosThetaO * cosThetaO < eta * eta ? Fresnel(cosThetaO, eta) : 1.0;
    if (cosThetaI >= 0.0) //a reflection light
    {
        const Vec3 H = (wi + wo).normalize();
        const Real cosThetaH = std::max(normal.dot(H), 0.0);
        Vec3 ret = 0.25 * Fr * color * surface->NormalDistribution(cosThetaH)
            / cosThetaO * surface->ShadowMasking(cosThetaI, cosThetaO) / cosThetaI;
        return ret;
    }
    else //a refraction light
    {
        cosThetaI = -cosThetaI;
        const Vec3 H = (wi * eta + wo).normalize();
        const Real cosThetaH = std::abs(normal.dot(H));
        Real HdotWi = H.dot(wi);
        Real HdotWo = std::abs(H.dot(wo));
        const Real tmp = (HdotWo + eta * HdotWi);
        Vec3 ret = (1.0 - Fr) * std::abs(HdotWi) / cosThetaI * HdotWo / cosThetaO
            * surface->NormalDistribution(cosThetaH) * color / (tmp * tmp) 
            * surface->ShadowMasking(cosThetaI, cosThetaO);
        return ret;
    }
}
