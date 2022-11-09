//
// Created by creeper on 22-10-28.
//
#include "Scene.h"
void Scene::load(const std::vector<Object*>& objList)
{
for (auto obj : objList)
    ObjList.push_back(obj);
}
void Scene::load(Object* obj)
{
    ObjList.push_back(obj);
}
Intersection Scene::intersect(const Ray& ray)
{
    Intersection ret;
    for (auto obj : ObjList)
    {
        Intersection inter = obj->intersect(ray);
        if (inter.hasIntersection && (!ret.hasIntersection || ret.dis >= inter.dis))
            ret = inter;
    }
    return ret;
}
void Scene::init()
{
    std::sort(ObjList.begin(), ObjList.end(), ObjectPtrCmp);
    BVHTree = new BVH(ObjList);
}
//Regulation: the direction of the ray must be flipped before checking and sampling
Spectrum Scene::cast_ray(Ray& ray)
{
    Spectrum throughput(1.0);
    Spectrum ret;
    Vec3& orig = ray.orig;
    Vec3& wo = ray.dir;
    for(int bounce = 0;; bounce++)
    {
        Intersection inter = intersect(ray);
        if (!inter.hasIntersection)
        {
            if(bounce) break;
            else return envMap->evalEmission(wo);
        }
        Material* mat = inter.mat;
        const Vec3& N = inter.normal;
        Mat3 TBN = construct_frame(inter.normal);
        wo = -wo;
        world_to_local(TBN, wo);
        if (wo[2] < 0.0 && !mat->translucent) break;
        TextureGroup* tex = inter.tex;
        const Vec3& P = inter.P;
        Real pdf_inv;
        Vec3 wi = mat->sample(wo, pdf_inv);
        Intersection dir_inter = intersect(Ray(P, wi));
        if (!dir_inter.hasIntersection)//calc the direct radiance from the environment/sky
        {
            Spectrum dir_rad = envMap->evalEmission(wi);
            Real cosThetaI = std::abs(N.dot(wi));
            ret += throughput * pdf_inv * mat->BxDF(wi, wo) * cosThetaI * dir_rad;//Using Monte Carlo Integration
        }
        if (get_random() > PRR) break;//sample at a probability of PRR and terminate at a probability of 1-PRR
        wi = mat->sample(N, wo, pdf_inv);//calculate the indirect radiance
        const Real cosThetaI = std::abs(N.dot(wi));
        throughput *= mat->BxDF(wi, wo) * pdf_inv * cosThetaI / PRR;
        orig = P;
        wo = wi;
    }
    return ret;
}