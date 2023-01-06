#ifndef RENDERCRAFT_BOX_H
#define RENDERCRAFT_BOX_H
#include "Shape.h"
#include "Intersection.h"
#include "Ray.h"
class Box : public Shape
{
	public:
		void intersect(const Ray& ray, Intersection *intsct) const override;
	private:
		Vec3 pMin, pMax;
};
#endif