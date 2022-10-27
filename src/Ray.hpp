#ifndef RENDERCRAFT_RAY_HPP
#define RENDERCRAFT_RAY_HPP
#include "../XMath/ext/Graphics/MathUtils.h"
struct Ray
{
    Vec3 orig, dir;
    Ray() = default;
    Ray(const Vec3& _orig, const Vec3& _dir) : orig(_orig), dir(_dir){}
    Ray(Vec3&& _orig, const Vec3& _dir) : orig(std::move(_orig)), dir(_dir){}
    Ray(const Vec3& _orig, Vec3&& _dir) : orig(_orig), dir(std::move(_dir)){}
    Ray(Vec3&& _orig, Vec3&& _dir) : orig(std::move(_orig)), dir(std::move(_dir)){}
    Vec3 operator()(Real t) const {return orig + t * dir;}
};
#endif