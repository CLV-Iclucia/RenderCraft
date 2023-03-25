#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Surface.h"

/**
 * Basic light interfaces.
 */
class Light
{
    public:
        virtual Spectrum evalEmission(const Vec3& pos, const Vec3& dir) const = 0;
        virtual Real pdfSample(const Vec3& pos) const = 0;
        virtual Vec3 sample(const Vec3& pos) const = 0;
};

class SurfaceLight : public Surface, public Light
{
    private:

    public:
        Spectrum evalEmission(const Vec3& pos, const Vec3& dir) const override;

};

class DirectLight : public Light
{

};

#endif