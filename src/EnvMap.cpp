//
// Created by creeper on 22-10-28.
//

#include "EnvMap.h"

Spectrum Sky::evalEmission(const Vec3 &world_dir) const
{
    return lerp(ColorA, ColorB, fabs(world_dir[1]));
}
