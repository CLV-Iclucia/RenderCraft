#ifndef RENDERCRAFT_OBJECT_H
#define RENDERCRAFT_OBJECT_H
#include <utility>

#include "Intersection.h"
#include "Ray.h"
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Shape.h"

class Object
{
	public:
		Object() = default;
		explicit Object(Vec3 _p, Material* _mat) : p(std::move(_p)), mat(_mat) {}
		Intersection intersect(const Ray&) const;
		virtual ~Object() = default;
		Real getX() const { return p[0]; }

	protected:
		Vec3 p;
        Shape* shape = nullptr;
        Material* mat = nullptr;
        TextureGroup* tex = nullptr;
};
inline bool ObjectPtrCmp(Object* A, Object* B)
{
	return A->getX() < B->getX();
}
#endif