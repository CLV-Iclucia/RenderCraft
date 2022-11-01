//
// Created by creeper on 22-10-31.
//
#include "Intersection.h"

void Intersection::CalcRayDifferential(const RayDiff &ray) const
{
    if(ray.hasDiff)
    {
        Real d = -P.dot(normal);
        Vec3 Px = ray.rx(-(ray.rx.orig.dot(normal) - d) / ray.rx.dir.dot(normal));
        Vec3 Py = ray.ry((-ray.ry.orig.dot(normal) - d) / ray.ry.dir.dot(normal));
        ray.dpdx = Px - P;
        ray.dpdy = Py - P;


    }
}