#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include "Ray.h"
#include "Box.h"
#include "Intersection.h"
#include <vector>
#include "Object.h"
struct BoundingVolume
{
	public:
		bool intersect(const Ray&) const;
		Vec3 pMin, pMax;
};
struct Node
{
	BoundingVolume B;
	Node* lch = nullptr, *rch = nullptr;
	Object* obj = nullptr;
};
class BVH
{
	public:
		explicit BVH(const std::vector<Object*>&);
		void intersect(const Ray&, Intersection *intsct) const;
	private:
		Node* rt;
};

#endif