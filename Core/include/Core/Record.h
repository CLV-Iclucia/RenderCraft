#ifndef RENDERCRAFT_RECORD_H
#define RENDERCRAFT_RECORD_H
#include <utility>

#include <Core/Material.h>
#include <Core/Texture.h>
#include <Core/Ray.h>
#include <Core/Light.h>

namespace rdcraft {
class Primitive;
struct Record
{
  Vec3 pos;
  Primitive* pr;

};

struct SurfaceRecord : public Record
{
  Vec3 normal; ///< coord of intersection point and corresponding normal
  Vec2 uv; ///< tex coord of the intersection point
  Vec3 dpdu, dpdv;
  Vec3 dndu, dndv;
  Real computeScatter(const Vec3& wi, const Vec3& wo) const;
};

struct VolumeRecord : public Record
{

};
}
#endif