#ifndef RENDERCRAFT_RECORD_H
#define RENDERCRAFT_RECORD_H
#include <utility>
#include <Core/Light.h>
#include <Core/Material.h>
#include <Core/Ray.h>
#include <Core/Texture.h>


namespace rdcraft {
class Primitive;
struct Record {
  Vec3 pos;
  Primitive *pr;
};

struct SurfaceRecord : public Record {
  Vec3 normal; ///< coord of intersection point and corresponding normal
  Vec2 uv;     ///< tex coord of the intersection point
  Vec3 dpdu, dpdv;
  Vec3 dndu, dndv;
  int refraction_rate = 1;
};

struct VolumeRecord : public Record {

};
} // namespace rdcraft
#endif