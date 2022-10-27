#ifndef RENDERCRAFT_BOX_H
#define RENDERCRAFT_BOX_H
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
#endif