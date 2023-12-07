#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include <Core/ray.h>
#include <Core/record.h>
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
class BVH : NonCopyable {
public:
  BVH(const MemoryManager<Primitive> &primitives);
  bool intersect(const Ray &, SurfaceRecord *pRec) const;
  bool intersect(const Ray &) const;
  AABB getAABB() const { return rt->bbox; }
  void recursiveBuild(std::unique_ptr<BVHNode>& o, const MemoryManager<Primitive> &primitives, int l, int r);

private:
  enum { ByIndex } divStrategy;
  std::unique_ptr<BVHNode> rt;
};

struct LBVHNode {
  AABB bbox;
  int l, r;
  int size;
};
class LBVH : NonCopyable {
 public:
  LBVH(const MemoryManager<Primitive> &primitives);
  bool intersect(const Ray &, SurfaceRecord *pRec) const;
  bool intersect(const Ray &) const;
  AABB getAABB() const { return nodes[0].bbox; }

 private:
  std::vector<LBVHNode> nodes;
};
} // namespace rdcraft
#endif