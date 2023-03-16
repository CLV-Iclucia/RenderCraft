//
// Created by creeper on 22-10-28.
//
#include <cassert>
#include "Scene.h"
void Scene::intersect(const Ray& ray, Intersection *intsct)
{
    assert(intsct->hasIntersection == false);
    for (auto obj : shapes)
    {
        Intersection inter;
        obj->intersect(ray, &inter);
        if (inter.hasIntersection && (!intsct->hasIntersection || intsct->dis >= inter.dis))
            *intsct = inter;
    }
}
void Scene::init()
{
    std::sort(shapes.begin(), shapes.end(), ObjPtrCmp);
    BVHTree = new BVH(ObjList);
}
//Regulation: the direction of the ray must be flipped before checking and sampling
Spectrum Scene::cast_ray(Ray& ray)
{

}