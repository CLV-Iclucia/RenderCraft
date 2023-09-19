#include <cassert>
#include "BVH.h"
#include "Primitive.h"
static bool intersectBox(const BBox3& bbox, const Ray& ray)
{
    const Vec3& orig = ray.orig;
    const Vec3& dir = ray.dir;
    const Vec3 invDir = 1.0 / dir;
    const Vec3b dirIsNeg = Vec3b({ dir[0] > 0.0, dir[1] > 0.0, dir[2] > 0.0, });
    Vec3 tMin = (bbox.lower - orig) * invDir;
    Vec3 tMax = (bbox.upper - orig) * invDir;
    if (!dirIsNeg[0]) std::swap(tMin[0], tMax[0]);
    if (!dirIsNeg[1]) std::swap(tMin[1], tMax[1]);
    if (!dirIsNeg[2]) std::swap(tMin[2], tMax[2]);
    const Real t_in = std::max(std::max(tMin[0], tMin[1]), tMin[2]);
    const Real t_out = std::min(std::min(tMax[0], tMax[1]), tMax[2]);
    if (t_in > t_out || t_out < 0.0) return false;
    else return true;
}
void build(BVHNode*& o, const std::vector<std::shared_ptr<Primitive>>& primitives, int l, int r)
{
    o = new BVHNode;
    if (l == r)
    {
        o->pr = primitives[l];
        o->bbox = primitives[l]->getBBox();
        return;
    }
    int mid = (l + r) >> 1;
    build(o->lch, primitives, l, mid);
    if(mid < r)build(o->rch, primitives, mid + 1, r);
    if(o->lch && !o->rch) o->bbox = o->lch->bbox;
    if(o->rch && !o->lch) o->bbox = o->rch->bbox;
    if(o->lch && o->rch)
        o->bbox = o->lch->bbox.merge(o->rch->bbox);
}
BVH::BVH(const std::vector<std::shared_ptr<Primitive>>& primitives)
{
    assert(!shapes.empty());
    build(rt, primitives, 0, primitives.size() - 1);
}

static bool intersect(const BVHNode* o, const Ray& ray, SurfaceRecord *intsct)
{
    if (o->lch == nullptr && o->rch == nullptr)
        o->pr->intersect(ray, intsct);
    else if (o->rch == nullptr && o->lch->B.intersect(ray))
        intersect(o->lch, ray, intsct);
    else if (o->lch == nullptr && o->rch->B.intersect(ray))
        intersect(o->rch, ray, intsct);
    else
    {
        bool intL = o->lch->B.intersect(ray);
        bool intR = o->rch->B.intersect(ray);
        if (intL && !intR) return intersect(o->lch, ray, intsct);
        else if (!intL && intR) return intersect(o->rch, ray, intsct);
        else
        {
            SurfaceInteraction interL;
            intersect(o->lch, ray, &interL);
            intersect(o->rch, ray, intsct);
            if (interL.hasIntersection && (!intsct->hasIntersection || interL.dis < intsct->dis))
                *intsct = interL;
        }
    }
}
bool BVH::intersect(const Ray& ray, SurfaceRecord *intsct) const
{
    if(intersectBox(rt->pr->getBBox(), ray))
        return ::intersect(rt, ray, intsct);
}

bool BVH::intersect(const Ray &) const
{
    return false;
}
