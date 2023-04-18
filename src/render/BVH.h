#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include "Ray.h"
#include "Record.h"
#include <vector>
#include "Object.h"
struct BVHNode
{
	BBox3 bbox;
	BVHNode* lch = nullptr, *rch = nullptr;
    std::shared_ptr<Primitive> pr;
};
class BVH
{
	public:
		bool intersect(const Ray&, SurfaceRecord *pRec) const;
        bool intersect(const Ray&) const;
	private:
		BVHNode* rt;
};

#endif