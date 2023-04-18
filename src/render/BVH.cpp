#include <cassert>
#include "BVH.h"

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
void build(BVHNode*& o, const std::vector<std::shared_ptr<Shape>>& shapes, int l, int r)
{
    o = new BVHNode;
    Real MinX = 1e9, MaxX = -1e9;
    Vec3 pMin(1e9), pMax(-1e9);
    for (int i = l; i <= r; i++)
    {
        auto obj = shapes[i];
        MinX = std::min(obj->getX(), obj->getX());
        MaxX = std::max(obj->getX(), obj->getX());
        Vec3 _pMax = obj->getCoordMax();
        for(int j = 0; j < 3; j++)
            pMin[j] = std::min(pMin[j], _pMin[j]);
        for(int j = 0; j < 3; j++)
            pMax[j] = std::max(pMax[j], _pMax[j]);
    }
    o->B.pMin = pMin;
    o->B.pMax = pMax;
    if (l == r)
    {
        o->obj = shapes[l];
        return;
    }
    Real MidX = (MinX + MaxX) * 0.5;
    int L = l, R = r;
    while (L < R)
    {
        int mid = (L + R) >> 1;
        if (shapes[mid]->getX() < MidX) L = mid + 1;
        else R = mid;
    }
    build(o->lch, shapes, l, L);
    if(L < r)build(o->rch, shapes, L + 1, r);
}
BVH::BVH(const std::vector<std::shared_ptr<Primitive>>& shapes)
{
    assert(!shapes.empty());
    build(rt, shapes, 0, shapes.size() - 1);
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
    if(intersectBox(rt->bbox, ray))
        return ::intersect(rt, ray, intsct);
}

bool BVH::intersect(const Ray &) const
{
    return false;
}
