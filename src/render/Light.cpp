//
// Created by creeper on 22-10-26.
//
#include "./Light.h"
#include "../XMath/ext/Graphics/MathUtils.h"

Spectrum DiskLight::evalEmission(const Vec3 &world_dir, const Vec3& p) const
{
    if(world_dir.dot(normal) > 0.0)
        return radiance;
    else return {};
}

Real DiskLight::pdf_inv(const Vec3 &ref, const Vec3& p) const
{
    return PI * radius * radius;
}

Vec3 DiskLight::sample(const Vec3 &) const
{
    Vec2 smp = uniform_sample_disk();
    return frame * Vec3({smp[0], smp[1], 0.0}) * radius + pos;
}

Spectrum SphereLight::evalEmission(const Vec3 &world_dir, const Vec3& p) const
{
    return radiance;
}

Real SphereLight::pdf_inv(const Vec3& ref, const Vec3 &p) const
{
    return PI4;
}

Vec3 SphereLight::sample(const Vec3 &) const
{
    return Vec3();
}
