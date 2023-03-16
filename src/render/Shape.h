//
// Created by creeper on 22-11-14.
//
#ifndef RENDERCRAFT_SHAPE_H
#define RENDERCRAFT_SHAPE_H
#include "../../XMath/ext/Vector.h"
#include "Ray.h"
#include "Intersection.h"
using ext::Vec3;
using ext::Real;
/**
 * Basic Shape interfaces.
 */
class Shape
{
    public:
        /**
         * sample a point on the surface of the shape given the position of the shape
         * @param p the world coordinates of the position
         * @return the coordinates of the sample point
         */
        virtual Vec3 sample(Real *pdf) const = 0;
        /**
         * calc the intersection with a given ray
         * @param ray a ray in the local space
         * @param p the world coordinates of the position
         * @return an Intersection class recording the info of this intersection
         */
        virtual void intersect(const Ray& ray, Intersection *intsct) const = 0;
        virtual Vec3 getLocalCoordMin() const = 0;
		virtual Vec3 getLocalCoordMax() const = 0;
        /**
         * calc the surface area of the shape
         * @return the are of the surface of the shape
         */
        virtual Real calcArea() const = 0;
        /**
         * calc the area of the visible region seen from the reference point
         * @param ref the coordinate reference
         * @param p
         * @return
         */
        virtual Real calcVisibleArea(const Vec3& ref) const = 0;
        virtual Vec3 sampleVisiblePoint(const Vec3& ref, Real *pdf) const = 0;
        virtual ~Shape() = default;
};

inline bool shapePtrCmp(Shape* A, Shape* B)
{
    return A->getX() < B->getX();
}

#endif //RENDERCRAFT_SHAPE_H
