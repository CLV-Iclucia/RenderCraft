//
// Created by creeper on 23-3-16.
//

#include "Integrator.h"
#include "rd_maths.h"
#include "Record.h"
#include "Primitive.h"

Spectrum PathTracer::nextEventEst(const Vec3& pos, ) const
{

    return {};
}

Spectrum PathTracer::L(const Ray &ray) const
{
    Spectrum throughput(1.0);
    Spectrum ret;
    Vec3 wo = ray.dir;
    for(int bounce = 0; ; bounce++)
    {
        if (get_random() > scene->opt.PRR) break;
        SurfaceRecord rec;
        if (!(scene->pr->intersect(ray, &rec)) || rec.pr->isLight())
        {
            if(bounce) break;
            else
            {
                if(scene->envMap)
                    ret += scene->envMap->evalEmission(wo);
                break;
            }
        }
        Material* mat = rec.pr->getMaterial();
        Mat3 TBN = constructFrame(rec.normal);
        wo = -ray.dir;
        wo = TBN * wo;
        const Vec2& uv = rec.uv;
        const Vec3& P = rec.pos;
        Real pdf;
        Vec3 wi = mat->sample(wo, &pdf, uv);
        L += throughput * nextEventEst(P);
        if (!(scene->pr->intersect(Ray(P, wi))))
        {
            Spectrum dir_rad = scene->envMap->evalEmission(wi);
            Real cosThetaI = std::abs(wi.z);
            ret += throughput * mat->BxDF(wi, wo, uv) * cosThetaI * dir_rad / pdf;//Using Monte Carlo Integration
        }

        wi = mat->sample(wo, &pdf, uv);
        wi = TBN.inv() * wi; // calculate the indirect radiance
        const Real cosThetaI = std::abs(wi[2]);
        throughput *= mat->BxDF(wi, wo, uv) / pdf * cosThetaI / scene->opt.PRR;
        wo = wi;
    }
    return ret;
}



// TODO: Implement volumetric path tracing
Spectrum VolumePathTracer::L(const Ray &ray) const
{

}
