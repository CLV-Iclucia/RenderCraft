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
Vec3 Scene::cast_ray(const Ray& ray, int depth)
{
    Intersection inter = intersect(ray);
    Vec3 wo = ray.dir;
    if (!inter.hasIntersection)
    {
        if(depth) return {};
        else return envMap->evalEmission(wo);
    }
    Material* mat = inter.mat;
    const Vec3& N = inter.normal;
    wo = -wo;
    if (checkInside(wo, N) && !mat->translucent) return {};
    const Vec3& P = inter.P;
    Vec3 ret;
    Real pdf_inv;
    Vec3 wi = mat->sample(N, wo, pdf_inv);
    Vec3 perturb_P = checkInside(wi, N) ? P - N * EPS : P + N * EPS;
    Intersection dir_inter = intersect(Ray(perturb_P, wi));
    if (!dir_inter.hasIntersection)//calc the direct radiance from the environment/sky
    {
        const Vec3 dir_rad = envMap->evalEmission(wi);
        const Real cosThetaI = std::abs(N.dot(wi));
        ret += pdf_inv * mat->BxDF(wi, wo, N) * cosThetaI * dir_rad;//Using Monte Carlo Integration
    }
    if (get_random() > PRR) return ret;//sample at a probability of PRR and terminate at a probability of 1-PRR
    wi = mat->sample(N, wo, pdf_inv);//calculate the indirect radiance
    const Real cosThetaI = std::abs(N.dot(wi));
    perturb_P = checkInside(wi, N) ? P - N * EPS : P + N * EPS;
    return ret + mat->BxDF(wi, wo, N) * pdf_inv * cosThetaI * cast_ray(Ray(perturb_P, wi), depth + 1) / PRR;
}