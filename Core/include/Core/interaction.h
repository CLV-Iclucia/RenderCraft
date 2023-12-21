#ifndef RENDERCRAFT_RECORD_H
#define RENDERCRAFT_RECORD_H

#include <Core/light.h>
#include <Core/material.h>

namespace rdcraft {
class Primitive;

struct Interaction {
  Vec3 pos;
  const Primitive* pr = nullptr;
  virtual ~Interaction();
};

struct SurfaceInteraction : Interaction {
  Vec3 normal;
  Vec2 uv;
};

struct ShadingInfo {
  Mat3 TBN;
  Vec3 wo, wi{};
  Vec2 uv;
  ShadingInfo(const SurfaceInteraction& si, const Vec3& wo_)
    : TBN(constructFrame(si.normal)), wo(TBN * wo_), uv(si.uv) {
  }
};

struct VolumeInteraction : Interaction {
};
} // namespace rdcraft
#endif