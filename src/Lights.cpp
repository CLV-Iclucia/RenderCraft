//
// Created by creeper on 22-10-26.
//
#include "./Lights.h"
#include "../XMath/MyMath.h"
Spectrum Sky::evalEmission(const Vec3& world_dir) const
{
    return lerp(ColorA, ColorB, std::fabs(world_dir[1]));
}

Spectrum DiskLight::evalEmission(const Vec3 &) const
{
    return Spectrum();
}

Real DiskLight::pdf_inv(const Vec3 &) const
{
    return 0;
}

Vec3 DiskLight::sample(const Vec3 &) const
{
    return Vec3();
}

Spectrum SphereLight::evalEmission(const Vec3 &world_dir) const
{
    return radiance;
}

Real SphereLight::pdf_inv(const Vec3 &p) const
{
    return PI4_INV;
}

Vec3 SphereLight::sample(const Vec3 &) const
{
    return Vec3();
}
