#pragma once
#include "Object.h"
#include "Intersection.h"
#include "Ray.hpp"
class Sphere : public Object
{
	public:
		Sphere(const Vec3& _p, Real _R, Material* m) : Object(_p, m), R(_R) {}
		Sphere(const Vec3& _p, Real _R) : Object(_p), R(_R) {}
		Intersection intersect(const Ray&) const override;
		Vec3 getCoordMin() const override;
		Vec3 getCoordMax() const override;
	private:
		Real R = 0.0;
};