#ifndef RENDERCRAFT_OBJECT_H
#define RENDERCRAFT_OBJECT_H
#include <utility>

#include "Intersection.h"
#include "Ray.hpp"
#include "../XMath/ext/Graphics/MathUtils.h"
class Object
{
	public:
		Object() = default;
		explicit Object(Vec3 _p) : p(std::move(_p)) {}
		Object(Vec3 _p, Material* m) : p(std::move(_p)), mat(m) {}
		virtual Intersection intersect(const Ray& ray) const = 0;
		virtual ~Object() = default;
		Real getX() const { return p[0]; }
		virtual Vec3 getCoordMin() const = 0;
		virtual Vec3 getCoordMax() const = 0;
	protected:
		Vec3 p;
		Material* mat = nullptr;
};
inline bool ObjectPtrCmp(Object* A, Object* B)
{
	return A->getX() < B->getX();
}
#endif