#ifndef RENDERCRAFT_MICROFACET_H
#define RENDERCRAFT_MICROFACET_H
#include <utility>
#include <texture.h>

#include <optional>

namespace rdcraft {
class Sampler;
struct BxdfSampleRecord {
  Vec3 wi;
  Real pdf;
};
struct Microfacet {
  public:
    virtual Real NormalDistribution(Real, const Vec2& uv) const = 0;
    virtual std::optional<BxdfSampleRecord> sample(const Vec2&, Sampler&) const
    = 0;
    virtual Real SmithMonoShadow(Real, const Vec2&) const = 0;
    virtual Real ShadowMasking(Real, Real, const Vec2&) const = 0;
    virtual ~Microfacet();
};

class TrowbridgeModel : public Microfacet {
  public:
    explicit TrowbridgeModel(std::unique_ptr<Texture<Real, 2>> alpha)
      : alpha(std::move(alpha)) {
    }
    std::optional<BxdfSampleRecord>
    sample(const Vec2&, Sampler&) const override;
    Real NormalDistribution(Real, const Vec2&) const override;
    Real SmithMonoShadow(Real, const Vec2&) const override;
    Real ShadowMasking(Real, Real, const Vec2&) const override;

  private:
    std::unique_ptr<Texture<Real, 2>> alpha;
};
}
#endif