#ifndef RENDERCRAFT_OBJECT_H
#define RENDERCRAFT_OBJECT_H
#include <utility>

#include "Intersection.h"
#include "Ray.h"
#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Shape.h"
#include "../../XMath/ext/Matrix.h"
#include "Transform.h"

class Object
{
	public:
		Object() = default;
		void intersect(const Ray&, Intersection *intsct) const;
        Object(const Transform& local2World, Shape* _shape, Material* _mat, TextureGroup* _tex) :
            Local2World(local2World), World2Local(local2World.inv()), shape(_shape), mat(_mat), tex(_tex) {}
		virtual ~Object() = default;
        Vec3 getCoordMin();
        Vec3 getCoordMax();
		Real getX() const { return Local2World.translate[0]; }

	protected:
        Transform Local2World;
        Transform World2Local;
        Shape* shape = nullptr;
        Material* mat = nullptr;
        TextureGroup* tex = nullptr;
};
inline bool ObjectPtrCmp(Object* A, Object* B)
{
	return A->getX() < B->getX();
}
#endif