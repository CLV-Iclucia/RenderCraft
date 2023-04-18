//
// Created by creeper on 23-3-16.
//

#include "Integrator.h"
#include "rd_maths.h"

Spectrum PathTracer::L(const Ray &ray) const
{
    Spectrum throughput(1.0);
    Spectrum ret;
    Vec3 orig = ray.orig;
    Vec3 wo = ray.dir;
    for(int bounce = 0; ; bounce++)
    {
        SurfaceRecord rec;
        if (!scene->pr->intersect(ray, &rec))
        {
            if(bounce) break;
            else return scene->envMap->evalEmission(wo);
        }
        Material* mat = rec.pr->getMaterial();
        Mat3 TBN = constructFrame(rec.normal);
        wo = -wo;
        wo = TBN * wo;
        Vec2& uv = rec.uv;
        const Vec3& P = rec.pos;
        Real pdf;
        Vec3 wi = mat->sample(wo, &pdf, uv);
        wi = TBN.inv() * wi;
        if (!scene->pr->intersect(Ray(P, wi)))//calc the direct radiance from the environment/sky
        {
            Spectrum dir_rad = scene->envMap->evalEmission(wi);
            Real cosThetaI = std::abs(wi[2]);
            ret += throughput * mat->BxDF(wi, wo, uv) * cosThetaI * dir_rad / pdf;//Using Monte Carlo Integration
        }
        if (get_random() > scene->opt.PRR) break; // sample at a probability of PRR and terminate at a probability of 1-PRR
        wi = mat->sample(wo, &pdf, uv);
        wi = TBN.inv() * wi; // calculate the indirect radiance
        const Real cosThetaI = std::abs(wi[2]);
        throughput *= mat->BxDF(wi, wo, uv) / pdf * cosThetaI / scene->opt.PRR;
        orig = P;
        wo = wi;
    }
    return ret;
}
