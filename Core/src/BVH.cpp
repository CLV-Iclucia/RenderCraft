#include <cassert>
#include <Core/BVH.h>
#include <Core/Primitive.h>
namespace rdcraft {
static AABB mergeAABB(const AABB &lhs, const AABB &rhs) {
  return {Vec3(std::min(lo(lhs).x, lo(rhs).x),
               std::min(lo(lhs).y, lo(rhs).y),
               std::min(lo(lhs).z, lo(rhs).z)),
          Vec3(std::max(hi(lhs).x, hi(rhs).x),
               std::max(hi(lhs).y, hi(rhs).y),
               std::max(hi(lhs).z, hi(rhs).z))};
}
static bool intersect(const AABB &bbox, const Ray &ray) {
  const Vec3 &orig = ray.orig;
  const Vec3 &dir = ray.dir;
  const Vec3 invDir = 1.0 / dir;
  const Vec3b dirIsNeg = Vec3b(dir[0] > 0.0, dir[1] > 0.0, dir[2] > 0.0);
  Vec3 tMin = (lo(bbox) - orig) * invDir;
  Vec3 tMax = (hi(bbox) - orig) * invDir;
  if (!dirIsNeg[0]) std::swap(tMin[0], tMax[0]);
  if (!dirIsNeg[1]) std::swap(tMin[1], tMax[1]);
  if (!dirIsNeg[2]) std::swap(tMin[2], tMax[2]);
  const Real t_in = std::max(std::max(tMin[0], tMin[1]), tMin[2]);
  const Real t_out = std::min(std::min(tMax[0], tMax[1]), tMax[2]);
  if (t_in > t_out || t_out < 0.0) return false;
  else return true;
}
void build(BVHNode *&o, const std::vector<std::shared_ptr<Primitive>> &primitives, int l, int r) {
  o = new BVHNode;
  if (l == r) {
    o->pr = primitives[l];
    o->bbox = primitives[l]->getAABB();
    return;
  }
  int mid = (l + r) >> 1;
  build(o->lch, primitives, l, mid);
  if (mid < r)build(o->rch, primitives, mid + 1, r);
  if (o->lch && !o->rch) o->bbox = o->lch->bbox;
  if (o->rch && !o->lch) o->bbox = o->rch->bbox;
  if (o->lch && o->rch)
    o->bbox = mergeAABB(o->lch->bbox, o->rch->bbox);
}
BVH::BVH(const std::vector<std::shared_ptr<Primitive>> &primitives) {
  assert(!primitives.empty());
  build(rt, primitives, 0, primitives.size() - 1);
}

static bool intersect(const BVHNode *o, const Ray &ray, SurfaceRecord *intsct) {
  if (o->lch == nullptr && o->rch == nullptr)
    return o->pr->intersect(ray, intsct);
  else if (o->rch == nullptr && intersect(o->lch->bbox, ray))
    return intersect(o->lch, ray, intsct);
  else if (o->lch == nullptr && intersect(o->rch->bbox, ray))
    return intersect(o->rch, ray, intsct);
  else {
    bool intL = intersect(o->lch->bbox, ray);
    bool intR = intersect(o->rch->bbox, ray);
    if (intL && !intR) 
      return intersect(o->lch, ray, intsct);
    else if (!intL && intR) 
      return intersect(o->rch, ray, intsct);
    else if (intL && intR) {
      SurfaceRecord record;
      if (intersect(o->lch, ray, intsct)) return true;
      else if (intersect(o->rch, ray, intsct)) return true;
      else return false;
    } else
      return false;
  }
}

bool BVH::intersect(const Ray &ray, SurfaceRecord *record) const {
  if (rdcraft::intersect(rt->pr->getAABB(), ray))
    return rdcraft::intersect(rt, ray, record);
  else return false;
}

bool BVH::intersect(const Ray &) const {
  return false;
}
}

