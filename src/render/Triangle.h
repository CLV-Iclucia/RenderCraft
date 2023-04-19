#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
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
        void intersect(const Ray& ray, SurfaceInteraction *intsct) const override;
        Triangle() = default;
        Vec3 getLocalCoordMin() const override;
		Vec3 getLocalCoordMax() const override;
        Real calcArea() const override;
        Vec3 sample(Real *pdf) const override;
        Real calcVisibleArea(const Vec3& ref) const override;
        Vec3 sampleVisiblePoint(const Vec3& ref, Real *pdf) const override;
    private:
        Vec3 v[3];
        Vec2 uv[3];
        Vec3 n[3];
        std::shared_ptr<Mesh> mesh = nullptr;///< the mesh that the triangle belongs to
};

#endif