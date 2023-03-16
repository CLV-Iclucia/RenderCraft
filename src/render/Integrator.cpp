//
// Created by creeper on 23-3-16.
//

#include "Integrator.h"

Spectrum PathTracer::castRay(const Ray &ray) const
{
    Spectrum throughput(1.0);
    Spectrum ret;
    Vec3 orig = ray.orig;
    Vec3 wo = ray.dir;
    for(int bounce = 0; ; bounce++)
    {
        Intersection intsct;
        intersect(ray, &intsct);
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
        intersect(Ray(P, wi), &dir_intsct);
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

Spectrum PathTracer::render(uint x, uint y) const
{
    Vec3 color;
    for (int k = 0; k < opt.spp; k++)
    {
        Vec2 offset = filter->sample();
        const Real rx = (x + offset[0]) * opt.ScrWid / camera.nx - opt.ScrWid * 0.5;
        const Real ry = (y + offset[1]) * opt.ScrHeight / camera.ny - opt.ScrHeight * 0.5;
        Vec3 dir({rx, ry, -1});
        dir.normalize();
        Ray ray({0, 0, 0}, dir);
        color += castRay(ray) / filter->pdfSample(offset[0], offset[1]);
    }
    color /= Real(opt.spp);
    if (color[0] >= 1.0)color[0] = 1.0;
    if (color[1] >= 1.0)color[1] = 1.0;
    if (color[2] >= 1.0)color[2] = 1.0;
    color[0] = std::sqrt(color[0]);
    color[1] = std::sqrt(color[1]);
    color[2] = std::sqrt(color[2]);
}