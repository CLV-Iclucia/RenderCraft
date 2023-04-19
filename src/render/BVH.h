#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include "Ray.h"
#include "Record.h"
#include <vector>

class Primitive;
struct BVHNode
{
    // TODO: Add default destructor for BoundingBox in MX3D
    BBox3 bbox;
    BVHNode() = default;
	BVHNode* lch = nullptr, *rch = nullptr;
    std::shared_ptr<Primitive> pr = nullptr;
};
class BVH
{
	public:
		bool intersect(const Ray&, SurfaceRecord *pRec) const;
        bool intersect(const Ray&) const;
        BBox3 getBBox() const { return rt->bbox; }
	private:
        enum { ByIndex } divStrategy;
		BVHNode* rt;
};

#endif