#ifndef RENDERCRAFT_MICROFACET_H
#define RENDERCRAFT_MICROFACET_H
#include "../XMath/ext/Graphics/MathUtils.h"
struct Microfacet
{
	public:
		virtual Real NormalDistribution(Real) const = 0;
		virtual Real SmithMonoShadow(Real) const = 0;
		virtual Real ShadowMasking(Real, Real) const = 0;
		virtual Vec3 ImportanceSample(Real&) const = 0;//(x, y, z, pdf_inv)
};
//struct BeckmannModel : public Microfacet
//{
//	Real NormalDistribution(const Vec3&) const override;
//	Real ShadowMasking(const Vec3&, const Vec3&, const Vec3&) const override;
//};
class TrowbridgeModel : public Microfacet
{
	public:
		explicit TrowbridgeModel(Real _alpha) : alpha(_alpha) {}
		Real NormalDistribution(Real) const override;
		Real SmithMonoShadow(Real) const override;
		Real ShadowMasking(Real, Real) const override;
		Vec3 ImportanceSample(Real&) const override;
	private:
		Real alpha = 1.0;
};
#endif