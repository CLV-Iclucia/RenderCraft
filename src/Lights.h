#ifndef RENDERCRAFT_LIGHTS_H
#define RENDERCRAFT_LIGHTS_H
#include "../XMath/ext/Graphics/MathUtils.h"
class Lights
{
    protected:
        Spectrum radiance;
    public:
        virtual Spectrum evalEmission(const Vec3&) const = 0;
        virtual Real pdf_inv(const Vec3&) const = 0;
        virtual Vec3 sample(const Vec3&) const = 0;
};

///<! Once Sky light is used, it will automatically return the radiance if the ray do not intersect with any objects.
class Sky
{
    private:
        Spectrum ColorA, ColorB;// in our setting, the color of the sky will lerp between the two colors.
    public:
        Spectrum evalEmission(const Vec3&) const;
};

/**
 * @brief DiskLight is a surface light in the shape of a round disk
 * pos is the coordinate of the center of the disk, and the disk faces normal.
 */
class DiskLight : Lights
{
    private:
        Real radius = 0.0;
        Vec3 pos, normal;
    public:
        Spectrum evalEmission(const Vec3&) const override;
        Real pdf_inv(const Vec3&) const override;
        Vec3 sample(const Vec3&) const override;
};

/**
 * @brief SphereLight is a sphere emitting light
 * pos is the coordinate of the center of the sphere.
 */
class SphereLight : Lights
{
    private:
        Real radius = 0.0;
        Vec3 pos;
    public:
        Spectrum evalEmission(const Vec3&) const override;
        Real pdf_inv(const Vec3&) const override;
        Vec3 sample(const Vec3&) const override;
};
#endif