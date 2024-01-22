#ifndef RENDERCRAFT_MICROFACET_H
#define RENDERCRAFT_MICROFACET_H
#include <Core/texture.h>
#include <utility>
#include <optional>

namespace rdcraft {
class Sampler;
struct BxdfSampleRecord {
  Vec3 local_wi;
  Real pdf;
};
struct Microfacet {
  virtual Real NormalDistribution(Real, const Vec2& uv) const = 0;
  virtual std::optional<BxdfSampleRecord> sampleNormal(const Vec2&, Sampler&) const
  = 0;
  virtual Real pdfSample(const Vec3& H, const Vec2& uv) const = 0;
  virtual Real SmithMonoShadow(Real, const Vec2&) const = 0;
  virtual Real ShadowMasking(Real, Real, const Vec2&) const = 0;
  virtual ~Microfacet();
};

class TrowbridgeModel final : public Microfacet {
  public:
    explicit TrowbridgeModel(std::unique_ptr<Texture<Real, 2>> alpha)
      : alpha(std::move(alpha)) {
    }
    std::optional<BxdfSampleRecord>
    sampleNormal(const Vec2&, Sampler&) const override;
    Real pdfSample(const Vec3& H, const Vec2& uv) const override;
    Real NormalDistribution(Real, const Vec2&) const override;
    Real SmithMonoShadow(Real, const Vec2&) const override;
    Real ShadowMasking(Real, Real, const Vec2&) const override;

  private:
    std::unique_ptr<Texture<Real, 2>> alpha;
};
}
#endif