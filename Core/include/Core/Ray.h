#ifndef RENDERCRAFT_RAY_H
#define RENDERCRAFT_RAY_H

#include "core.h"
struct Ray
{
    Vec3 orig, dir;
    Ray() = default;
    Ray(const Vec3& _orig, const Vec3& _dir) : orig(_orig), dir(_dir){}
    Ray(Vec3&& _orig, const Vec3& _dir) : orig(std::move(_orig)), dir(_dir){}
    Ray(const Vec3& _orig, Vec3&& _dir) : orig(_orig), dir(std::move(_dir)){}
    Ray(Vec3&& _orig, Vec3&& _dir) : orig(std::move(_orig)), dir(std::move(_dir)){}
    Vec3 operator()(Real t) const { return orig + t * dir; }
};

/**
 * \brief this follows the method of pbrt-v3, use ray differential technique in path tracing
 */
struct RayDiff : public Ray
{
    ///By default, hasDiff is enabled.
    bool hasDiff = true;///< whether ray differential is enabled
    mutable Vec3 dpdx, dpdy;///< the partial derivative of the intersection point to the screen coord
    mutable Real dudx = 0, dudy = 0, dvdx = 0, dvdy = 0;///<the partial derivative of the tex coord to screen coord
    mutable Ray rx, ry;
};
#endif