#ifndef RENDERCRAFT_BOX_H
#define RENDERCRAFT_BOX_H
#include "Shape.h"
#include "Intersection.h"
#include "Ray.h"
class Box : public Shape
{
	public:
		Intersection intersect(const Ray& ray, const Vec3& p) const override;
	private:
		Vec3 pMin, pMax;
};
#endif