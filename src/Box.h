#pragma once
#include "Object.h"
#include "Intersection.h"
#include "Ray.hpp"
class Box : Object
{
	public:
		Intersection intersect(const Ray&) const override;
	private:
		Vec3 pMin, pMax;
};