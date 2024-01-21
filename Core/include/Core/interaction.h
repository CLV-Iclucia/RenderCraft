#ifndef RENDERCRAFT_RECORD_H
#define RENDERCRAFT_RECORD_H

#include <Core/light.h>
#include <Core/material.h>

namespace rdcraft {
class Primitive;

struct Interaction {
  Interaction() = default;
  Interaction(const Vec3& pos, const Primitive* pr)
    : pos(pos), pr(pr) {
  }
  Vec3 pos{};
  const Primitive* pr = nullptr;
  virtual ~Interaction() = default;
};

struct SurfaceInteraction final : Interaction {
  SurfaceInteraction() = default;
  SurfaceInteraction(const Vec2& uv, const Vec3& pos, const Vec3& normal)
    : Interaction(pos, nullptr), normal(normal), uv(uv) {
  }
  Vec3 normal{};
  Vec2 uv{};
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