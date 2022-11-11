#ifndef RENDERCRAFT_MATERIAL_H
#define RENDERCRAFT_MATERIAL_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Microfacet.h"
#include <utility>
struct Material
{
    virtual Vec3 BxDF(const Vec3&, const Vec3&, const Vec2&) const = 0;
    virtual Vec3 sample(const Vec3&, Real&, const Vec2&) const = 0;
};
class Lambertian : public Material //Lambertian diffuse
{
    public:
        Lambertian(Real R, Real G, Real B) : albedo({ R, G, B }) {}
        explicit Lambertian(Vec3 col) : albedo(std::move(col)) {}
        Vec3 sample(const Vec3&, Real&, const Vec2&) const override;
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec2&) const override;
    private:
        std::shared_ptr<Texture<Vec3> > albedo = {1.0, 1.0, 1.0};
};
class Metal : public Material//using Torrance-Sparrow Model
{
    public:
        Metal(Real etaR, Real etaG, Real etaB, Real kR, Real kG, Real kB, Microfacet* surf) :
            eta({etaR, etaG, etaB}), k({kR, kG, kB}), surface(surf), surface(surf) {}
        Metal(Vec3 _eta, Vec3 _k, Microfacet* surf) : eta(std::move(_eta)), k(std::move(_k)), surface(surf) {}
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec2&) const override;
        Vec3 sample(const Vec3&, Real&, const Vec2&) const override;
    private:
        Vec3 Fresnel(Real) const;
        Microfacet* surface;//surface = nullptr means that the surface is smooth
        Vec3 eta, k;// /bar{eta} = eta + ik according to PBR
};
//this class in fact describe a surface between two translucent media
class Translucent : public Material //translucent dielectrics
{
    public:
        Translucent(Real _eta, Microfacet* surf, Real R, Real G, Real B) : etaA(_eta),
            surface(surf), color({ R, G, B }), Material(true) {}
        Translucent(Real _eta, Microfacet* surf, Vec3 _color) : etaA(_eta),
            surface(surf), color(std::move(_color)), Material(true) {}
        Translucent(Real _eta, Vec3 _color) : etaA(_eta),
            surface(nullptr), color(std::move(_color)), Material(true) {}
        Translucent(Real _eta, Microfacet* surf) : etaA(_eta), surface(surf), color(1.0), Material(true) {}
        Vec3 sample(const Vec3&, Real&, const Vec2&) const override;
        Vec3 BxDF(const Vec3&, const Vec3&, const Vec2&) const override;
    private:
        static Real Fresnel(Real, Real);
        Microfacet* surface;//surface = nullptr means that the surface is smooth
        std::shared_ptr<Texture<Vec3> > color;
        Real etaA, etaB = 1.0;//etaA: eta of the media inside; etaB: eta of the media outside
};//by default etaB is set to 1, eta of the vacuum
#endif