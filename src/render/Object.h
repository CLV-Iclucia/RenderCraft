#ifndef RENDERCRAFT_OBJECT_H
#define RENDERCRAFT_OBJECT_H
#include <utility>

#include "Intersection.h"
#include "Ray.h"
#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Shape.h"
#include "../../XMath/ext/Matrix.h"

class Object
{
	public:
		Object() = default;
		explicit Object(Vec3 _p, Material* _mat) : p(std::move(_p)), mat(_mat) {}
		void intersect(const Ray&, Intersection *intsct) const;
		virtual ~Object() = default;
        Vec3 getCoordMin();
        Vec3 getCoordMax();
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