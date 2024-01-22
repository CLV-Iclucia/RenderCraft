#include <Core/bvh.h>
#include <Core/primitive.h>
#include <Core/utils.h>
#include <Core/parallel.h>
#include <cassert>
#include <cstdint>
#include <Core/memory.h>
#include <gtest/gtest.h>

namespace rdcraft {
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

// [l, r)
static void lbvhRecursiveBuild(std::unique_ptr<LbvhRawNode>& o,
                               const std::vector<MortonPrimitive>& mp,
                               const PolymorphicVector<Primitive>& primitives,
                               int l, int r, int depth) {
  ASSERT(l < r, "empty range");
  if (o == nullptr)
    o = std::make_unique<LbvhRawNode>();
  if (depth == 0 || r - l == 1) {
    o->start = l;
    o->end = r;
    o->bbox = primitives(l)->getAABB();
    for (int i = l + 1; i < r; i++)
      o->bbox = mergeAABB(o->bbox, primitives(i)->getAABB());
    return;
  }
  if (int bitMask = 1 << depth; (mp[l].mortonCode & bitMask) == (mp[r - 1].
                                mortonCode & bitMask)) {
    o->start = l;
    o->end = r;
    lbvhRecursiveBuild(o, mp, primitives, l, r, depth - 1);
  } else {
    int searchStart = l;
    int searchEnd = r - 1;
    while (searchStart < searchEnd) {
      int mid = (searchStart + searchEnd + 1) >> 1;
      if ((mp[mid].mortonCode & bitMask) == (mp[l].mortonCode & bitMask))
        searchStart = mid;
      else
        searchEnd = mid - 1;
    }
    int split = searchEnd + 1;
    o->start = l;
    o->end = r;
    o->axis = depth % 3;
    lbvhRecursiveBuild(o->lch, mp, primitives, l, split, depth - 1);
    lbvhRecursiveBuild(o->rch, mp, primitives, split, r, depth - 1);
    o->bbox = mergeAABB(o->lch->bbox, o->rch->bbox);
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
  int ltr_end = nodes.size() - 1;
  lbvhFlatten(o->rch, nodes);
  nodes[currentIdx].rchOffset = ltr_end + 1 - currentIdx;
}

void LBVH::lbvhBuild(PolymorphicVector<Primitive>&& unsortedPrimitives,
                     std::vector<LBVHNode>& nodes) {
  std::vector<LbvhBuildingInfo> buildingInfos;
  std::vector<MortonPrimitive> mortonPrimitives;
  buildingInfos.resize(unsortedPrimitives.size());
  mortonPrimitives.resize(unsortedPrimitives.size());
  AABB aggregateBbox(unsortedPrimitives(0)->getAABB());
  for (int i = 1; i < unsortedPrimitives.size(); i++)
    aggregateBbox = mergeAABB(aggregateBbox, unsortedPrimitives(i)->getAABB());
  // for all Primitives to compute their centroids and init bounding box
  for (int i = 0; i < unsortedPrimitives.size(); i++) {
    buildingInfos[i].bbox = unsortedPrimitives(i)->getAABB();
    Vec3 centroid = (lo(buildingInfos[i].bbox) + hi(buildingInfos[i].bbox)) *
                    0.5;
    buildingInfos[i].centroid = centroid;
    mortonPrimitives[i].primitiveIndex = i;
    Vec3 reScaledCentroid = (centroid - lo(aggregateBbox)) / (
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
  std::ranges::sort(mortonPrimitives, [](const auto& lhs, const auto& rhs) {
    return lhs.mortonCode < rhs.mortonCode;
  });
  // reorder primitives and mortonPrimitives
  primitives.get().resize(mortonPrimitives.size());
  for (int i = 0; i < mortonPrimitives.size(); i++) {
    primitives.get()[i] = std::move(
        unsortedPrimitives.get()[mortonPrimitives[i].primitiveIndex]);
  }
  std::unique_ptr<LbvhRawNode> rt{};
  lbvhRecursiveBuild(rt, mortonPrimitives, primitives, 0, primitives.size(),
                     31);
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
  if (!intersectBBox(nodes[0].bbox, ray))
    return;
  while (true) {
    const LBVHNode& node = nodes[nodeIdx];
    // it is a leaf node
    if (node.nPrimitives) {
      intersectLeaf(ray, nodeIdx, si);
      if (!top) return;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    const LBVHNode& lch = nodes[nodeIdx + 1];
    assert(nodeIdx + node.rchOffset < nodes.size());
    const LBVHNode& rch = nodes[nodeIdx + node.rchOffset];
    int axis = node.axis;
    bool intl = intersectBBox(lch.bbox, ray);
    bool intr = intersectBBox(rch.bbox, ray);
    if (!intl && !intr) {
      if (!top) return;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    if (intl && !intr) nodeIdx = nodeIdx + 1;
    else if (!intl) nodeIdx = nodeIdx + node.rchOffset;
    else {
      int lchIdx = nodeIdx + 1;
      int rchIdx = nodeIdx + node.rchOffset;
      if (ray.dir[axis] > 0.0) {
        nodeIdx = lchIdx;
        nodesToVisit[top++] = rchIdx;
      } else {
        nodeIdx = rchIdx;
        nodesToVisit[top++] = lchIdx;
      }
    }
  }
}

bool LBVH::intersect(const Ray& ray) const {
  std::array<int, 64> nodesToVisit{};
  int top{};
  int nodeIdx = 0;
  if (!intersectBBox(nodes[0].bbox, ray))
    return false;
  while (true) {
    const LBVHNode& node = nodes[nodeIdx];
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
    const LBVHNode& rch = nodes[nodeIdx + node.rchOffset];
    bool intl = intersectBBox(lch.bbox, ray);
    bool intr = intersectBBox(rch.bbox, ray);
    if (!intl && !intr) {
      if (!top) return false;
      nodeIdx = nodesToVisit[--top];
      continue;
    }
    if (intl && !intr) nodeIdx = nodeIdx + 1;
    else if (!intl) nodeIdx = nodeIdx + node.rchOffset;
    else {
      int lchIdx = nodeIdx + 1;
      int rchIdx = nodeIdx + node.rchOffset;
      if (ray.dir[node.axis] > 0.0) {
        nodeIdx = lchIdx;
        nodesToVisit[top++] = rchIdx;
      } else {
        nodeIdx = rchIdx;
        nodesToVisit[top++] = lchIdx;
      }
    }
  }
}
}