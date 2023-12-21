#include <Core/bvh.h>
#include <Core/primitive.h>
#include <Core/utils.h>
#include <Core/parallel.h>
#include <cassert>
#include <cstdint>
#include <Core/memory.h>

namespace rdcraft {
static bool intersectBBox(const AABB& bbox, const Ray& ray) {
  const Vec3& orig = ray.orig;
  const Vec3& dir = ray.dir;
  const Vec3 invDir = 1.0 / dir;
  const Vec3b dirIsNeg{dir[0] > 0.0, dir[1] > 0.0, dir[2] > 0.0};
  Vec3 tMin = (lo(bbox) - orig) * invDir;
  Vec3 tMax = (hi(bbox) - orig) * invDir;
  if (!dirIsNeg[0]) std::swap(tMin[0], tMax[0]);
  if (!dirIsNeg[1]) std::swap(tMin[1], tMax[1]);
  if (!dirIsNeg[2]) std::swap(tMin[2], tMax[2]);
  Real t_in = std::max(std::max(tMin[0], tMin[1]), tMin[2]);
  if (Real t_out = std::min(std::min(tMax[0], tMax[1]), tMax[2]);
    t_in > t_out || t_out < 0.0)
    return false;
  return true;
}
// void BVH::recursiveBuild(std::unique_ptr<BVHNode>& o,
//                          const MemoryManager<Primitive>& primitives,
//                          int l,
//                          int r) {
//   o = std::make_unique<BVHNode>();
//   if (l == r) {
//     o->pr = primitives(l);
//     o->bbox = primitives(l)->getAABB();
//     return;
//   }
//   int mid = (l + r) >> 1;
//   recursiveBuild(o->lch, primitives, l, mid);
//   if (mid < r) recursiveBuild(o->rch, primitives, mid + 1, r);
//   if (o->lch && !o->rch) o->bbox = o->lch->bbox;
//   if (o->rch && !o->lch) o->bbox = o->rch->bbox;
//   if (o->lch && o->rch)
//     o->bbox = mergeAABB(o->lch->bbox, o->rch->bbox);
// }

// BVH::BVH(const MemoryManager<Primitive>& primitives) {
//   assert(!primitives.empty());
//   recursiveBuild(rt, primitives, 0, primitives.size() - 1);
// }

// static bool intersect(const std::unique_ptr<BVHNode>& o, const Ray& ray,
//                       std::optional<SurfaceInteraction>& intsct) {
//   if (o->lch == nullptr && o->rch == nullptr)
//     return o->pr->intersect(ray, intsct);
//   if (o->rch == nullptr && intersect(o->lch->bbox, ray))
//     return intersect(o->lch, ray, intsct);
//   if (o->lch == nullptr && intersect(o->rch->bbox, ray))
//     return intersect(o->rch, ray, intsct);
//   bool intL = intersect(o->lch->bbox, ray);
//   bool intR = intersect(o->rch->bbox, ray);
//   if (intL && !intR)
//     return intersect(o->lch, ray, intsct);
//   if (!intL && intR)
//     return intersect(o->rch, ray, intsct);
//   if (intL && intR) {
//     if (intersect(o->lch, ray, intsct)) return true;
//     if (intersect(o->rch, ray, intsct)) return true;
//     return false;
//   }
//   return false;
// }
//
// void BVH::intersect(const Ray& ray, std::optional<SurfaceInteraction>& record) const {
//   if (rdcraft::intersect(rt->pr->getAABB(), ray))
//     rdcraft::intersect(rt, ray, record);
//   record = std::nullopt;
// }
//
// bool BVH::intersect(const Ray&) const {
//   return false;
// }

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
  AABB bbox;
  int start{}, end{};
  int axis{};
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
  if (int bitMask = 1 << depth; mp[l].mortonCode & bitMask == mp[r].mortonCode &
                                bitMask) {
    o->start = l;
    o->end = r;
    lbvhRecursiveBuild(o, mp, l, r, depth - 1);
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
    o->axis = depth % 3;
    lbvhRecursiveBuild(o->lch, mp, l, split, depth - 1);
    lbvhRecursiveBuild(o->rch, mp, split, r, depth - 1);
  }
}

static void lbvhFlatten(const std::unique_ptr<LbvhRawNode>& o,
                        std::vector<LBVHNode>& nodes) {
  if (o == nullptr)
    return;
  if (!o->lch && !o->rch) {
    nodes.emplace_back(o->bbox, o->start, o->end - o->start, o->axis);
    return;
  }
  assert(o->lch && o->rch);
  nodes.emplace_back(o->bbox, -1, 0, o->axis);
  int currentIdx = nodes.size() - 1;
  lbvhFlatten(o->lch, nodes);
  nodes[currentIdx].rchOffset = nodes.size() - 1 - currentIdx;
  lbvhFlatten(o->rch, nodes);
}

void lbvhBuild(const MemoryManager<Primitive>& primitives,
               std::vector<LBVHNode>& nodes) {
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
  lbvhFlatten(rt, nodes);
}

void LBVH::intersectLeaf(const Ray& ray, int leaf,
                         std::optional<SurfaceInteraction>& si) const {
  int begin = nodes[leaf].startOffset;
  int end = nodes[leaf].startOffset + nodes[leaf].nPrimitives;
  std::optional<SurfaceInteraction> si_tmp;
  for (int i = begin; i < end; i++) {
    primitives(i)->intersect(ray, si_tmp);
    if (si_tmp && (distance(si_tmp->pos, ray.orig) < distance(si->pos, ray.orig)
                   || !si))
      si = std::move(si_tmp);
  }
}

bool LBVH::intersectLeaf(const Ray& ray, int leaf) const {
  int begin = nodes[leaf].startOffset;
  int end = nodes[leaf].startOffset + nodes[leaf].nPrimitives;
  for (int i = begin; i < end; i++)
    if (primitives(i)->intersect(ray))
      return true;
  return false;
}

void LBVH::intersect(const Ray& ray,
                     std::optional<SurfaceInteraction>& si) const {
  std::array<int, 64> nodesToVisit{};
  int top{};
  int nodeIdx = 0;
  si = std::nullopt;
  while (true) {
    const LBVHNode& node = nodes[nodeIdx];
    if (!intersectBBox(node.bbox, ray)) {
      if (!top) return;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    // it is a leaf node
    if (node.nPrimitives) {
      intersectLeaf(ray, nodeIdx, si);
      if (!top) return;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    const LBVHNode& lch = nodes[nodeIdx + 1];
    assert(node.rchOffset < nodes.size());
    const LBVHNode& rch = nodes[node.rchOffset];
    int axis = node.axis;
    bool intl = intersectBBox(lch.bbox, ray);
    bool intr = intersectBBox(rch.bbox, ray);
    if (!intl && !intr) {
      if (!top) return;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    if (intl && !intr) nodesToVisit[top++] = nodeIdx + 1;
    else if (!intl) nodesToVisit[top++] = node.rchOffset;
    else {
      if (ray.dir[axis] > 0.0) {
        nodeIdx = nodeIdx + 1;
        nodesToVisit[top++] = node.rchOffset;
      } else {
        nodeIdx = node.rchOffset;
        nodesToVisit[top++] = nodeIdx + 1;
      }
    }
  }
}

bool LBVH::intersect(const Ray& ray) const {
  std::array<int, 64> nodesToVisit{};
  int top{};
  int nodeIdx = 0;
  while (true) {
    const LBVHNode& node = nodes[nodeIdx];
    if (!intersectBBox(node.bbox, ray)) {
      if (!top) return false;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    // it is a leaf node
    if (node.nPrimitives) {
      if (intersectLeaf(ray, nodeIdx))
        return true;
      if (!top) return false;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    const LBVHNode& lch = nodes[nodeIdx + 1];
    assert(node.rchOffset < nodes.size());
    const LBVHNode& rch = nodes[node.rchOffset];
    bool intl = intersectBBox(lch.bbox, ray);
    bool intr = intersectBBox(rch.bbox, ray);
    if (!intl && !intr) {
      if (!top) return false;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    if (intl && !intr) nodeIdx = nodeIdx + 1;
    else if (!intl) nodeIdx = node.rchOffset;
    else {
      if (ray.dir[node.axis] > 0.0) {
        nodeIdx = nodeIdx + 1;
        nodesToVisit[top++] = node.rchOffset;
      } else {
        nodeIdx = node.rchOffset;
        nodesToVisit[top++] = nodeIdx + 1;
      }
    }
  }
}
}