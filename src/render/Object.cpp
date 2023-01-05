//
// Created by creeper on 22-11-14.
//
#include "Object.h"

Intersection Object::intersect(const Ray &ray, Intersection *intsct) const
{
    shape->intersect(ray, intsct);
    intsct->mat = mat;
}

Vec3 Object::getCoordMax()
{
    return p + shape->getLocalCoordMax();
}

Vec3 Object::getCoordMin()
{
    return p + shape->getLocalCoordMin();
}

