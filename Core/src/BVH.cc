#include <Core/BVH.h>
#include <Core/primitive.h>
#include <Core/maths.h>
#include <tbb/tbb.h>
#include <cassert>
#include <cstdint>
#include <Core/memory.h>

namespace rdcraft {
static bool intersect(const AABB& bbox, const Ray& ray) {
  const Vec3& orig = ray.orig;
  const Vec3& dir = ray.dir;
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
void BVH::recursiveBuild(std::unique_ptr<BVHNode>& o,
                         const MemoryManager<Primitive>& primitives,
                         int l,
                         int r) {
  o = std::make_unique<BVHNode>();
  if (l == r) {
    o->pr = primitives(l);
    o->bbox = primitives(l)->getAABB();
    return;
  }
  int mid = (l + r) >> 1;
  recursiveBuild(o->lch, primitives, l, mid);
  if (mid < r) recursiveBuild(o->rch, primitives, mid + 1, r);
  if (o->lch && !o->rch) o->bbox = o->lch->bbox;
  if (o->rch && !o->lch) o->bbox = o->rch->bbox;
  if (o->lch && o->rch)
    o->bbox = mergeAABB(o->lch->bbox, o->rch->bbox);
}

BVH::BVH(const MemoryManager<Primitive>& primitives) {
  assert(!primitives.empty());
  recursiveBuild(rt, primitives, 0, primitives.size() - 1);
}

static bool intersect(const std::unique_ptr<BVHNode>& o, const Ray& ray,
                      SurfaceRecord* intsct) {
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

bool BVH::intersect(const Ray& ray, SurfaceRecord* record) const {
  if (rdcraft::intersect(rt->pr->getAABB(), ray))
    return rdcraft::intersect(rt, ray, record);
  else return false;
}

bool BVH::intersect(const Ray&) const {
  return false;
}

constexpr int kMortonBits = 10;

struct LbvhBuildingInfo {
  AABB bbox;
  Vec3 centroid;
};

struct MortonPrimitive {
  int primitiveIndex;
  uint32_t mortonCode;
};

uint64_t mortonEncode(uint32_t x, uint32_t y, uint32_t z) {
  x = (x | (x << 16)) & 0x0000ffff0000ffff;
  x = (x | (x << 8)) & 0x00ff00ff00ff00ff;
  x = (x | (x << 4)) & 0x0f0f0f0f0f0f0f0f;
  x = (x | (x << 2)) & 0x3333333333333333;
  x = (x | (x << 1)) & 0x5555555555555555;

  y = (y | (y << 16)) & 0x0000ffff0000ffff;
  y = (y | (y << 8)) & 0x00ff00ff00ff00ff;
  y = (y | (y << 4)) & 0x0f0f0f0f0f0f0f0f;
  y = (y | (y << 2)) & 0x3333333333333333;
  y = (y | (y << 1)) & 0x5555555555555555;

  z = (z | (z << 16)) & 0x0000ffff0000ffff;
  z = (z | (z << 8)) & 0x00ff00ff00ff00ff;
  z = (z | (z << 4)) & 0x0f0f0f0f0f0f0f0f;
  z = (z | (z << 2)) & 0x3333333333333333;
  z = (z | (z << 1)) & 0x5555555555555555;

  return x | (y << 1) | (z << 2);
}

struct LbvhRawNode {
  int start, end;
  std::unique_ptr<LbvhRawNode> lch{}, rch{};
};

static void lbvhRecursiveBuild(std::unique_ptr<LbvhRawNode>& o,
                               const std::vector<MortonPrimitive>& mp,
                               int l, int r, int depth) {
  if (o == nullptr)
    o = std::make_unique<LbvhRawNode>();
  if (depth == 0) {
    o->start = l;
    o->end = r;
    return;
  }
  int bitMask = 1 << depth;
  if (mp[l].mortonCode & bitMask == mp[r].mortonCode & bitMask) {
    o->start = l;
    o->end = r;
    lbvhRecursiveBuild(o->lch, mp, l, r, depth - 1);
  } else {
    int searchStart = l;
    int searchEnd = r;
    while (searchStart + 1 < searchEnd) {
      int mid = (searchStart + searchEnd) >> 1;
      if (mp[mid].mortonCode & bitMask == mp[l].mortonCode & bitMask)
        searchStart = mid;
      else
        searchEnd = mid;
    }
    int split = searchEnd;
    o->start = l;
    o->end = r;
    lbvhRecursiveBuild(o->lch, mp, l, split - 1, depth - 1);
    lbvhRecursiveBuild(o->rch, mp, split, r, depth - 1);
  }
}

static void lbvhFlatten() {

}

static void lbvhBuild(const MemoryManager<Primitive>& primitives) {
  std::vector<LbvhBuildingInfo> buildingInfos;
  std::vector<MortonPrimitive> mortonPrimitives;
  buildingInfos.reserve(primitives.size());
  mortonPrimitives.reserve(primitives.size());
  AABB aggregateBbox(primitives(0)->getAABB());
  for (int i = 1; i < primitives.size(); i++)
    aggregateBbox = mergeAABB(aggregateBbox, primitives(i)->getAABB());
  // parallel for all Primitives to compute their centroids and init bounding box
  tbb::parallel_for
      (tbb::blocked_range<size_t>(0, primitives.size()),
       [&](const tbb::blocked_range<size_t>& range) {
         for (size_t i = range.begin(); i < range.end(); i++) {
           buildingInfos[i].bbox = primitives(i)->getAABB();
           Vec3 centroid = (lo(buildingInfos[i].bbox) + hi(
                                buildingInfos[i].bbox)) * 0.5;
           buildingInfos[i].centroid = centroid;
           mortonPrimitives[i].primitiveIndex = i;
           Vec3 reScaledCentroid =
               (centroid - lo(aggregateBbox)) / (
                 hi(aggregateBbox) - lo(aggregateBbox));
           mortonPrimitives[i].mortonCode =
               mortonEncode(
                   static_cast<uint64_t>(
                     reScaledCentroid.x * (1 << kMortonBits)),
                   static_cast<uint64_t>(
                     reScaledCentroid.y * (1 << kMortonBits)),
                   static_cast<uint64_t>(
                     reScaledCentroid.z * (1 << kMortonBits)));
         }
       });
  tbb::parallel_sort
      (mortonPrimitives.begin(), mortonPrimitives.end(),
       [](const MortonPrimitive& lhs, const MortonPrimitive& rhs) {
         return lhs.mortonCode < rhs.mortonCode;
       });
  std::unique_ptr<LbvhRawNode> rt{};
  lbvhRecursiveBuild(rt, mortonPrimitives, 0, primitives.size(), 31);
  lbvhFlatten(rt);
}

LBVH::LBVH(const MemoryManager<Primitive>& primitives) {
  assert(!primitives.empty());
  lbvhBuild(primitives);
}

bool LBVH::intersect(const Ray& ray, SurfaceRecord* record) const {

}

bool LBVH::intersect(const Ray& ray) const {
}
}