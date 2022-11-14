//
// Created by creeper on 22-11-14.
//
#ifndef RENDERCRAFT_SHAPE_H
#define RENDERCRAFT_SHAPE_H
#include "../XMath/ext/Vector.h"
#include "Ray.hpp"
#include "Intersection.h"
using ext::Vec3;
class Shape
{
    public:
        virtual Vec3 sample(const Vec3&) = 0;
        virtual Intersection intersect(const Ray&, const Vec3&) const = 0;
        virtual Vec3 getLocalCoordMin() const = 0;
		virtual Vec3 getLocalCoordMax() const = 0;
        virtual ~Shape() = default;
};


#endif //RENDERCRAFT_SHAPE_H
