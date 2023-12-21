#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include <Core/ray.h>
#include <Core/interaction.h>
#include <Core/memory.h>
#include <vector>

namespace rdcraft {
class Primitive;
struct BVHNode : NonCopyable {
  AABB bbox;
  BVHNode() = default;
  std::unique_ptr<BVHNode> lch = nullptr, rch = nullptr;
  const Primitive* pr = nullptr;
};
// class BVH : NonCopyable {
//   public:
//     BVH(const MemoryManager<Primitive>& primitives);
//     bool intersect(const Ray&, SurfaceInteraction* pRec) const;
//     bool intersect(const Ray&) const;
//     AABB getAABB() const { return rt->bbox; }
//     void recursiveBuild(std::unique_ptr<BVHNode>& o,
//                         const MemoryManager<Primitive>& primitives, int l,
//                         int r);
//
//   private:
//     enum { ByIndex } divStrategy;
//     std::unique_ptr<BVHNode> rt;
// };

struct LBVHNode {
  AABB bbox;
  union {
    int startOffset;
    int rchOffset;
  };
  int nPrimitives;
  uint8_t axis;
  LBVHNode(AABB bbox, int offset, int nPrimitives, uint8_t axis)
    : bbox(std::move(bbox)), startOffset(offset), nPrimitives(nPrimitives),
      axis(axis) {
  }
};

void lbvhBuild(const MemoryManager<Primitive>& primitives,
                      std::vector<LBVHNode>& nodes);
class LBVH : NonCopyable {
  public:
    explicit LBVH(MemoryManager<Primitive>&& primitives)
      : primitives(std::move(primitives)) {
      ASSERT(!primitives.empty(), "primitives is empty");
      lbvhBuild(primitives, nodes);
    }
    void intersect(const Ray&, std::optional<SurfaceInteraction>& si) const;
    bool intersect(const Ray&) const;
    AABB getAABB() const { return nodes[0].bbox; }

  private:
    void intersectLeaf(const Ray&, int leaf,
                       std::optional<SurfaceInteraction>& si) const;
    bool intersectLeaf(const Ray& ray, int leaf) const;
    std::vector<LBVHNode> nodes;
    MemoryManager<Primitive> primitives;
};
} // namespace rdcraft
#endif