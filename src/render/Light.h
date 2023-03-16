#ifndef RENDERCRAFT_LIGHT_H
#define RENDERCRAFT_LIGHT_H

#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Shape.h"

/**
 * Basic light interfaces.
 */
class Light
{
    public:
        virtual Spectrum evalEmission(const Vec3&, const Vec3&) const = 0;
        virtual Real pdf_inv(const Vec3&, const Vec3&) const = 0;
        virtual Vec3 sample(const Vec3&) const = 0;
};

class ShapeLight : public Light, public Shape
{

};

#endif