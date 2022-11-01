#include "BVH.h"
bool BoundingVolume::intersect(const Ray& ray) const
{
    const Vec3& orig = ray.orig;
    const Vec3& dir = ray.dir;
    const Vec3 invDir = 1.0 / dir;
    const Vec3b dirIsNeg = Vec3b({ dir[0] > 0.0, dir[1] > 0.0, dir[2] > 0.0, });
    Vec3 tMin = (pMin - orig) * invDir;
    Vec3 tMax = (pMax - orig) * invDir;
    if (!dirIsNeg[0])std::swap(tMin[0], tMax[0]);
    if (!dirIsNeg[1])std::swap(tMin[1], tMax[1]);
    if (!dirIsNeg[2])std::swap(tMin[2], tMax[2]);
    const Real t_in = std::max(std::max(tMin[0], tMin[1]), tMin[2]);
    const Real t_out = std::min(std::min(tMax[0], tMax[1]), tMax[2]);
    if (t_in > t_out || t_out < 0.0) return false;
    else return true;
}
BVH::BVH(const std::vector<Object*>& ObjList)
{
    rt->build(rt, ObjList, 0, ObjList.size() - 1);
}
void Node::build(Node*& o, const std::vector<Object*>& ObjList, int l, int r)
{
    o = new Node;
    Real MinX = 1e9, MaxX = -1e9;
    Vec3 pMin(1e9), pMax(-1e9);
    for (unsigned int i = l; i <= r; i++)
    {
        auto obj = ObjList[i];
        MinX = std::min(obj->getX(), obj->getX());
        MaxX = std::max(obj->getX(), obj->getX());
        Vec3 _pMin = obj->getCoordMin();
        Vec3 _pMax = obj->getCoordMax();
        for(int i = 0; i < 3; i++)
            pMin[i] = std::min(pMin[i], _pMin[i]);
        for(int i = 0; i < 3; i++)
            pMax[i] = std::max(pMin[i], _pMin[i]);
    }
    o->B.pMin = pMin;
    o->B.pMax = pMax;
    if (l == r)
    {
        o->obj = ObjList[l];
        return;
    }
    Real MidX = (MinX + MaxX) * 0.5;
    int L = l, R = r;
    while (L < R)
    {
        int mid = (L + R) >> 1;
        if (ObjList[mid]->getX() < MidX) L = mid + 1;
        else R = mid;
    }
    build(o->lch, ObjList, l, L);
    if(L < r)build(o->rch, ObjList, L + 1, r);
}
Intersection BVH::intersect(const Node* o, const Ray& ray)
{
    if (o->lch == nullptr && o->rch == nullptr)
        return o->B.intersect(ray) ? o->obj->intersect(ray) : Intersection();
    else if (o->rch == nullptr) 
        return o->lch->B.intersect(ray) ? intersect(o->lch, ray) : Intersection();
    else if (o->lch == nullptr)
        return o->rch->B.intersect(ray) ? intersect(o->rch, ray) : Intersection();
    else
    {
        bool intL = o->lch->B.intersect(ray);
        bool intR = o->rch->B.intersect(ray);
        if (intL && !intR) return intersect(o->lch, ray);
        else if (!intL && intR) return intersect(o->rch, ray);
        else
        {
            Intersection interL = intersect(o->lch, ray);
            Intersection interR = intersect(o->rch, ray);
            if (!interL.hasIntersection && !interR.hasIntersection) return {};
            else if (interL.hasIntersection && !interR.hasIntersection) return interL;
            else if (!interL.hasIntersection && interR.hasIntersection) return interR;
            else return interL.dis < interR.dis ? interL : interR;
        }
    }
}
