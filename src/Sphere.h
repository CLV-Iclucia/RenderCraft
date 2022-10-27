#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include "Object.h"
#include "Intersection.h"
#include "Ray.hpp"
class Sphere : public Object
{
	public:
		Sphere(const Vec3& _p, Real r, Material* m) : Object(_p, m), R(r) {}
		Sphere(const Vec3& _p, Real r) : Object(_p), R(r) {}
		Intersection intersect(const Ray&) const override;
		Vec3 getCoordMin() const override;
		Vec3 getCoordMax() const override;
	private:
		Real R = 0.0;
};
#endif