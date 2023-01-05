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
		explicit Sphere(Real r) : R(r) {}
        bool isConvex() const override { return true; }
        void intersect(const Ray&, Intersection *intsct) const override;
		Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
        Real calcArea() const override;
        Vec3 sample(Real& pdf) const override;
        Real calcVisibleArea(const Vec3& ref) const override;
        Vec3 sampleVisiblePoint(const Vec3& ref, Real *pdf) const override;
	private:
		Real R = 0.0;
};
#endif