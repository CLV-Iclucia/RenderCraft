#ifndef RENDERCRAFT_SPHERE_H
#define RENDERCRAFT_SPHERE_H
#include "Shape.h"
#include "Intersection.h"
#include "Ray.h"
class Sphere : public Shape
{
	public:
		explicit Sphere(Real r) : R(r) {}
        bool isConvex() const override { return true; }
        Intersection intersect(const Ray&, const Vec3&) const override;
		Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
        Real calcArea() const override;
        Vec3 sample(const Vec3& p) const override;
        Real calcVisibleArea(const Vec3 &ref, const Vec3 &p) const override;
        Vec3 sampleVisiblePoint(const Vec3&, const Vec3&, Real&) const override;
	private:
		Real R = 0.0;
};
#endif