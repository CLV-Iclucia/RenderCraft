#pragma once
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Microfacet.h"
#include "../XMath/ext/Graphics/MathUtils.h"
struct Material
{
    bool translucent;
    explicit Material(bool _translucent) { translucent = _translucent; }
    virtual Vec3 BxDF(const Vec3&, const Vec3&, const Vec3&) const = 0;
    virtual Vec3 eval(const Vec3&, const Vec3&, const Vec3&) const = 0;//calc the Monte Carlo Integration Term
    virtual Vec3 sample(const Vec3&, const Vec3&, Real&) const = 0;
};
class Lambertian : public Material //Lambertian diffuse
{
    public:
        Lambertian(Real R, Real G, Real B) : albedo({ R, G, B }), Material(false) {}
        explicit Lambertian(const Vec3& col) : albedo(col), Material(false) {}
        Vec3 eval(const Vec3&, const Vec3&, const Vec3&) const override;
        Vec3 sample(const Vec3&, const Vec3&, Real&) const override;
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec3&) const override;
    private:
        Vec3 albedo = {1.0, 1.0, 1.0};
};
class Metal : public Material//using Torrance-Sparrow Model
{
    public:
        Metal(Real etaR, Real etaG, Real etaB, Real kR, Real kG, Real kB, Microfacet* surf) :
            eta({etaR, etaG, etaB}), k({kR, kG, kB}), surface(surf), Material(false) {}
        Metal(const Vec3& _eta, const Vec3& _k, Microfacet* surf) : eta(_eta), k(_k), surface(surf), Material(false) {}
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec3&) const override;
        Vec3 sample(const Vec3&, const Vec3&, Real&) const override;
        Vec3 eval(const Vec3&, const Vec3&, const Vec3&) const override;
    private:
        Vec3 Fresnel(Real) const;
        Microfacet* surface;//surface = nullptr means that the surface is smooth
        Vec3 eta, k;// /bar{eta} = eta + ik according to PBR
};
//this class in fact describe a surface between two translucent media
class Translucent : public Material //translucent dielectrics
{
    public:
        static const bool translucent;
        Translucent(Real _eta, Microfacet* surf, Real R, Real G, Real B) : etaA(_eta),
            surface(surf), color({ R, G, B }), Material(true) {}
        Translucent(Real _eta, Microfacet* surf, const Vec3& _color) : etaA(_eta),
            surface(surf), color(_color), Material(true) {}
        Translucent(Real _eta, const Vec3& _color) : etaA(_eta),
            surface(nullptr), color(_color), Material(true) {}
        Translucent(Real _eta, Microfacet* surf) : etaA(_eta), surface(surf), color(1.0), Material(true) {}
        Vec3 sample(const Vec3&, const Vec3&, Real&) const override;
        Vec3 eval(const Vec3&, const Vec3&, const Vec3&) const override;
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec3&) const override;
    private:
        Real Fresnel(Real, Real) const;
        Microfacet* surface;//surface = nullptr means that the surface is smooth
        Vec3 color;
        Real etaA, etaB = 1.0;//etaA: eta of the media inside; etaB: eta of the media outside
};//by default etaB is set to 1, eta of the vacuum