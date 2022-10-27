#ifndef RENDERCRAFT_MYMATH_H
#define RENDERCRAFT_MYMATH_H
#include <random>
#include <cmath>
#include "../src/Vector.h"
#include "../src/global.h"
const Real PI = 3.1415926535897;
const Real PI2 = 6.28318530717958;
const Real PI_INV = 0.318309886183;
const Real PI2_INV = 0.159154943091895335;
const Real PI4_INV = 0.079577471546;
const Real EPS = 1e-13;
template<typename T>
T lerp(const T &A, const T &B, Real t)
{
    return t * A + (1.0 - t) * B;
}
inline Real get_random()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<Real> u(0, 1);
    return u(e);
}
inline Vec3 cross(const Vec3& A, const Vec3& B)
{
    return Vec3({ A[1] * B[2] - A[2] * B[1], A[2] * B[0] - A[0] * B[2], A[0] * B[1]- A[1] * B[0]});
}
inline Vec3 uniform_sample_hemisphere()//uniformly sample a point on a hemisphere x^2+y^2+z^2=1,z>=0
{
    const Real Phi = get_random() * PI2;
    const Real cosTheta = get_random();
    const Real sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    return Vec3({sinTheta * std::cos(Phi), sinTheta * std::sin(Phi), cosTheta});
}
inline Vec2 uniform_sample_disk()//uniformly sample a point on a disk x^2+y^2=1
{
    const Real Phi = get_random() * PI2;
    const Real R = std::sqrt(get_random());
    return Vec2({R * std::cos(Phi), R * std::sin(Phi)});
}
inline Vec3 cos_weighted_sample_hemisphere()//sample a point on a hemisphere x^2+y^2+z^2=1,z>=0 at cosine weight
{
    const Vec2 p = uniform_sample_disk();
    return Vec3({ p[0], p[1], std::sqrt(1 - p[0] * p[0] - p[1] * p[1]) });
}
inline bool fequal(Real a, Real b)
{
    return a - b <= EPS && a - b >= -EPS;
}
inline bool VectorEqual(const Vec3& a, const Vec3& b)
{
    return (a - b).norm() <= EPS;
}
//here N.dot(wo) > 0
inline Vec3 refract(const Vec3& wo, const Vec3& N, Real eta)//eta = etaI / etaO
{
    Real cosThetaO = std::min(N.dot(wo), 1.0);
    Real sinThetaO = std::sqrt(1.0 - cosThetaO * cosThetaO);
    Real sinThetaI = sinThetaO / eta;
    Real cosThetaI = std::sqrt(1.0 - sinThetaI * sinThetaI);
    Vec3 ver = cosThetaO * N - wo;
    Vec3 ret = -cosThetaI * N + ver / eta;
    return ret;
}
inline bool checkInside(const Vec3& wo, const Vec3& N)
{
    return wo.dot(N) <= 0.0;
}
#endif