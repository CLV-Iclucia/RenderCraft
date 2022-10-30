#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Object.h"

class Light : public Object
{
    protected:
        Spectrum radiance;
    public:
        virtual Spectrum evalEmission(const Vec3&, const Vec3&) const = 0;
        virtual Real pdf_inv(const Vec3&, const Vec3&) const = 0;
        virtual Vec3 sample(const Vec3&) const = 0;
};

/**
 * @brief DiskLight is a surface light in the shape of a round disk
 * pos is the coordinate of the center of the disk, and the disk faces normal.
 */
class DiskLight : public Light
{
    private:
        Mat3 frame;
        Real radius = 0.0;
        Vec3 pos, normal;
    public:
        DiskLight(Vec3 _pos, Vec3 _normal, Real _radius) : pos(std::move(_pos)), normal(std::move(_normal)),
                                                            radius(_radius), frame(construct_frame(normal)) {}
        Spectrum evalEmission(const Vec3&, const Vec3&) const override;
        Real pdf_inv(const Vec3&, const Vec3&) const override;
        Vec3 sample(const Vec3&) const override;
};

/**
 * @brief SphereLight is a sphere emitting light \n
 */
class SphereLight : public Light
{
    private:
        Real radius = 0.0;
        Vec3 pos;
    public:
        Spectrum evalEmission(const Vec3&, const Vec3&) const override;
        Real pdf_inv(const Vec3&, const Vec3&) const override;
        Vec3 sample(const Vec3&) const override;
};
#endif