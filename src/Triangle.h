#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Object.h"
#include "Material.h"
#include "Mesh.h"

class Triangle : public Shape
{
    public:
        Intersection intersect(const Ray &, const ext::Vec3 &) const override;
        Triangle() = default;
        Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
        Real calcVisibleArea(const Vec3&, const Vec3&) const override;
        Vec3 sample(const Vec3&, const Vec3& p, Real&) const override;
    private:
        Vec3 v[3];
        Vec2 uv[3];
        Mesh* mesh = nullptr;///< the mesh that the triangle belongs to
};

#endif