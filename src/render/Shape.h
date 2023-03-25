//
// Created by creeper on 22-11-14.
//
#ifndef RENDERCRAFT_SHAPE_H
#define RENDERCRAFT_SHAPE_H
#include <utility>

#include "../../XMath/ext/Vector.h"
#include "Ray.h"
#include "Intersection.h"
#include "types.h"

class Shape
{
    protected:
        Vec3 pos;
    public:
        explicit Shape(Vec3  _pos) : pos(std::move(_pos)) {}
        /**
         * sample a point on the surface of the shape given the position of the shape
         * @return the coordinates of the sample point
         */
        virtual Vec3 sample(Real *pdf) const = 0;
        /**
         * calc the intersection with a given ray
         * @param ray a ray in the local space
         * @return an Intersection class recording the info of this intersection
         */
        virtual void intersect(const Ray& ray, Intersection *intsct) const = 0;
        virtual Vec3 getCoordMin() const = 0;
		virtual Vec3 getCoordMax() const = 0;
        /**
         * calc the surface area of the shape
         * @return the are of the surface of the shape
         */
        virtual Real pdfSample(const Vec3& p) const = 0;
        virtual ~Shape() = default;
        Real getX() const { return pos[0]; }
        Real getY() const { return pos[1]; }
        Real getZ() const { return pos[2]; }
};



#endif //RENDERCRAFT_SHAPE_H
