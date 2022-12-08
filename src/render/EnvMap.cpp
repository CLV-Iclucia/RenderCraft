//
// Created by creeper on 22-10-28.
//

#include "EnvMap.h"

Spectrum Sky::evalEmission(const Vec3 &world_dir) const
{
    return lerp(ColorA, ColorB, 0.5 * world_dir[1] + 0.5);
}
