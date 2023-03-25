//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_CAMERA_H
#define RENDERCRAFT_CAMERA_H

#include "types.h"
#include "Ray.h"

struct Camera
{
    uint nx, ny;
    Vec3 pos;
    void castRay(const Vec3& dir, Ray* ray) const;
};


#endif //RENDERCRAFT_CAMERA_H
