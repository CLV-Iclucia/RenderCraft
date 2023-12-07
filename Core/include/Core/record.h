#ifndef RENDERCRAFT_RECORD_H
#define RENDERCRAFT_RECORD_H

#include <utility>
#include <Core/light.h>
#include <Core/Material.h>
#include <Core/ray.h>
#include <Core/Texture.h>

namespace rdcraft {
class Primitive;

struct Record {
  Vec3 pos;
  const Primitive *pr = nullptr;
};

struct SurfaceRecord : public Record {
  Vec3 normal;
  Vec2 uv;
};

struct VolumeRecord : public Record {

};

struct EndpointRecord : public Record {

};

} // namespace rdcraft
#endif