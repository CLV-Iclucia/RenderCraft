//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_TYPES_H
#define RENDERCRAFT_TYPES_H

#include <mx3d_Vector.h>
#include <mx3d_Matrix.h>
#include <mx3d_BoudingBox.h>
namespace core {
using uint = unsigned int;
using mx3d::Real;
using mx3d::Mat3;
using mx3d::Vec2f;
using mx3d::Vec2;
using mx3d::Vec3;
using mx3d::Vec3b;
using mx3d::Vec3f;
using mx3d::Vec3i;
using mx3d::BoundingBox;
using BBox3 = BoundingBox<3u>;
struct Patch
{
  Vec3 p;
  Vec3 n;
};
}

#endif //RENDERCRAFT_TYPES_H
