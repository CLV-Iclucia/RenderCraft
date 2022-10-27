#pragma once
#include "Vector.h"
struct Ray
{
    Vec3 orig, dir;
    Ray() = default;
    Ray(const Vec3& _orig, const Vec3& _dir) : orig(_orig), dir(_dir){}
    Vec3 operator()(Real t) const {return orig + t * dir;}
};