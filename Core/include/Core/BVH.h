#ifndef RENDERCRAFT_BVH_H
#define RENDERCRAFT_BVH_H
#include <Core/Ray.h>
#include <Core/Record.h>
#include <vector>
namespace rdcraft {
class Primitive;
struct BVHNode
{
  AABB bbox;
  BVHNode() = default;
  BVHNode* lch = nullptr, *rch = nullptr;
  std::shared_ptr<Primitive> pr = nullptr;
};
class BVH
{
 public:
  bool intersect(const Ray&, SurfaceRecord *pRec) const;
  bool intersect(const Ray&) const;
  AABB getAABB() const { return rt->bbox; }
 private:
  enum { ByIndex } divStrategy;
  BVHNode* rt;
};
}
#endif