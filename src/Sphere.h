#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include "Shape.h"
#include "Intersection.h"
#include "Ray.hpp"
class Sphere : public Shape
{
	public:
		explicit Sphere(Real r) : R(r) {}
        Intersection intersect(const Ray&, const Vec3&) const override;
		Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
	private:
		Real R = 0.0;
};
#endif