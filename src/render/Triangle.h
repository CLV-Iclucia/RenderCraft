#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Object.h"
#include "Material.h"
#include "Mesh.h"

/**
 * The triangle class
 *
 */
class Triangle : public Shape
{
    public:
        bool isConvex() const override { return true; }
        Intersection intersect(const Ray &, const ext::Vec3 &) const override;
        Triangle() = default;
        Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
        Real calcArea() const override;
        Vec3 sample() const override;
        Real calcVisibleArea(const Vec3&, const Vec3&) const override;
        Vec3 sampleVisiblePoint(const Vec3&, const Vec3& p, Real&) const override;
    private:
        Vec3 v[3];
        Vec2 uv[3];
        Vec3 n[3];
        Mesh* mesh = nullptr;///< the mesh that the triangle belongs to
};

#endif