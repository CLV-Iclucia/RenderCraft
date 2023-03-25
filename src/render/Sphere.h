#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include "Shape.h"
#include "Intersection.h"
#include "Ray.h"
#include "../../XMath/ext/Vector.h"
/**
 * Because all the coordinates are in local frame, the position of the sphere is on (0, 0, 0) by default
 */
class Sphere : public Shape
{
	public:
		Sphere(const Vec3& pos, Real r) : Shape(pos), R(r) {}
        void intersect(const Ray&, Intersection *intsct) const override;
		Vec3 getCoordMin() const override { return pos - Vec3{R, R, R}; }
		Vec3 getCoordMax() const override { return pos + Vec3{R, R, R}; }
        Vec3 sample(Real *pdf) const override;
	private:
		Real R = 0.0;
};
#endif