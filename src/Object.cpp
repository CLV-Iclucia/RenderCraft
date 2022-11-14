//
// Created by creeper on 22-11-14.
//
#include "Object.h"

Intersection Object::intersect(const Ray &ray) const
{
    Intersection inter = shape->intersect(ray, p);
    inter.mat = mat;
    return shape->intersect(ray, p);
}

