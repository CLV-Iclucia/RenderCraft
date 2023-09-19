//
// Created by creeper on 22-10-31.
//
#include "Record.h"

void SurfaceRecord::calcRayDiff(const RayDiff &ray) const
{
    if(ray.hasDiff)
    {
        Real d = -pos.dot(normal);
        Vec3 Px = ray.rx(-(ray.rx.orig.dot(normal) - d) / ray.rx.dir.dot(normal));
        Vec3 Py = ray.ry((-ray.ry.orig.dot(normal) - d) / ray.ry.dir.dot(normal));
        ray.dpdx = Px - pos;
        ray.dpdy = Py - pos;


    }
}