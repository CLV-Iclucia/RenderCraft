//
// Created by creeper on 22-11-14.
//
#include "Object.h"

void Object::intersect(const Ray &ray, SurfaceInteraction *intsct) const
{
    Ray local_ray(World2Local(ray.orig), World2Local(ray.dir));
    shape->intersect(local_ray, intsct);
    intsct->pos = Local2World(intsct->pos);
    intsct->pos = Local2World.rot * intsct->normal;
    intsct->mat = mat;
}

Vec3 Object::getCoordMax()
{
    return Local2World.translate + shape->getLocalCoordMax();
}

Vec3 Object::getCoordMin()
{
    return Local2World.translate + shape->getLocalCoordMin();
}

