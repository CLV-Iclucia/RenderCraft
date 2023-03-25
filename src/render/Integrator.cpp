//
// Created by creeper on 23-3-16.
//

#include "Integrator.h"
#include "../../XMath/ext/Graphics/MathUtils.h"

Spectrum PathTracer::L(const Ray &ray) const
{
    Spectrum throughput(1.0);
    Spectrum ret;
    Vec3 orig = ray.orig;
    Vec3 wo = ray.dir;
    for(int bounce = 0; ; bounce++)
    {
        Intersection intsct;
        scene.intersect(ray, &intsct);
        if (!intsct.hasIntersection)
        {
            if(bounce) break;
            else return scene.envMap->evalEmission(wo);
        }
        Material* mat = intsct.mat;
        Mat3 TBN = construct_frame(intsct.normal);
        wo = -wo;
        world_to_local(TBN, wo);
        TextureGroup* tex = intsct.tex;
        Vec2& uv = intsct.uv;
        const Vec3& P = intsct.P;
        Real pdf_inv;
        Vec3 wi = mat->sample(wo, &pdf_inv, uv);
        local_to_world(TBN, wi);
        Intersection dir_intsct;
        scene.intersect(Ray(P, wi), &dir_intsct);
        if (!dir_intsct.hasIntersection)//calc the direct radiance from the environment/sky
        {
            Spectrum dir_rad = scene.envMap->evalEmission(wi);
            Real cosThetaI = std::abs(wi[2]);
            ret += throughput * pdf_inv * mat->BxDF(wi, wo, uv) * cosThetaI * dir_rad;//Using Monte Carlo Integration
        }
        if (get_random() > opt.PRR) break; // sampleVisiblePoint at a probability of PRR and terminate at a probability of 1-PRR
        wi = mat->sample(wo, &pdf_inv, uv);
        local_to_world(TBN, wi); // calculate the indirect radiance
        const Real cosThetaI = std::abs(wi[2]);
        throughput *= mat->BxDF(wi, wo, uv) * pdf_inv * cosThetaI / opt.PRR;
        orig = P;
        wo = wi;
    }
    return ret;
}
