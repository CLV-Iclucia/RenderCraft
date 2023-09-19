//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_CAMERA_H
#define RENDERCRAFT_CAMERA_H

#include "Ray.h"
#include "core.h"

struct Camera
{
    uint nx, ny;
    Vec3 pos;
    void castRay(const Vec3& dir, Ray* ray) const
    {
        ray->orig = pos;
        ray->dir = dir.normalized();
    }
};


#endif //RENDERCRAFT_CAMERA_H
