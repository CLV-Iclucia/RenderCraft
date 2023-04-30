//
// Created by creeper on 22-11-14.
//
#ifndef RENDERCRAFT_SHAPE_H
#define RENDERCRAFT_SHAPE_H
#include <utility>

#include "Ray.h"
#include "types.h"
#include "Transform.h"

class SurfaceRecord;
class Shape
{
    protected:
        Transform World2Obj;
        Transform Obj2World;
    public:
        Shape() = default;
        /**
         * sample a point on the surface of the shape given the position of the shape
         * @return the coordinates of the sample point
         */
        virtual Patch sample(Real *pdf) const = 0;
        virtual Patch sample(const Vec3& ref, Real *pdf) const = 0;
        virtual Patch sample(const Vec3& ref) const = 0;
        /**
         * calc the intersection with a given ray
         * @param ray a ray in the local space
         * @return an Intersection class recording the info of this intersection
         */
        virtual bool intersect(const Ray& ray, SurfaceRecord *pRec) const = 0;
        virtual bool intersect(const Ray& ray) const = 0;
		virtual BBox3 getBBox() const = 0;
        /**
         * calc the surface area of the shape
         * @return the are of the surface of the shape
         */
        virtual Real pdfSample(const Vec3& p) const = 0;
        virtual Real pdfSample(const Vec3& p, const Vec3& ref) const = 0;
        virtual ~Shape() = default;
};



#endif //RENDERCRAFT_SHAPE_H
