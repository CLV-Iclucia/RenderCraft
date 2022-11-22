#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include "Ray.h"
#include "Box.h"
#include "Intersection.h"
#include <vector>
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
	void build(Node*&, const std::vector<Object*>&, int, int);
};
class BVH
{
	public:
		BVH(const std::vector<Object*>&);
		Intersection intersect(const Node*, const Ray&);
	private:
		Node* rt;
};

#endif