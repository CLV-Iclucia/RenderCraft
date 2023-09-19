#ifndef RENDERCRAFT_MICROFACET_H
#define RENDERCRAFT_MICROFACET_H
#include <utility>

#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Texture.h"
struct Microfacet
{
	public:
		virtual Real NormalDistribution(Real, const Vec2& uv) const = 0;
		virtual Real SmithMonoShadow(Real, const Vec2&) const = 0;
		virtual Real ShadowMasking(Real, Real, const Vec2&) const = 0;
		virtual Vec3 ImportanceSample(Real*, const Vec2&) const = 0;//(x, y, z, pdf_inv)
};
//struct BeckmannModel : public Microfacet
//{
//	Real NormalDistribution(const Vec3&) const override;
//	Real ShadowMasking(const Vec3&, const Vec3&, const Vec3&) const override;
//};
class TrowbridgeModel : public Microfacet
{
	public:
        explicit TrowbridgeModel(std::shared_ptr<Texture<Real> >  _alpha) : alpha(std::move(_alpha)) {}
		Real NormalDistribution(Real, const Vec2&) const override;
		Real SmithMonoShadow(Real, const Vec2&) const override;
		Real ShadowMasking(Real, Real, const Vec2&) const override;
		Vec3 ImportanceSample(Real*, const Vec2&) const override;
	private:
		std::shared_ptr<Texture<Real> > alpha;
        Vec3 SampleVNDF(Real *pdf_inv) const;
};

class NormalMapMicrofacet : public Microfacet
{
    public:
        explicit NormalMapMicrofacet(std::shared_ptr<Texture<Vec3> >& tex) : normal_map(tex) {}
        Real NormalDistribution(Real, const Vec2&) const override;
		Real SmithMonoShadow(Real, const Vec2&) const override;
		Real ShadowMasking(Real, Real, const Vec2&) const override;
    private:
        std::shared_ptr<Texture<Vec3> > normal_map;
};
#endif