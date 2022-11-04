#include "Sphere.h"
#include "../XMath/ext/Graphics/MathUtils.h"
#include <cmath>
Intersection Sphere::intersect(const Ray& ray) const
{
    const Vec3 dif = ray.orig - p;
    const Real B = dif.dot(ray.dir);
    const Real C = dif.dot(dif) - R * R;
    Real delta = B * B - C;
    Intersection inter;
    if (delta < 0.0) return inter;
    delta = std::sqrt(delta);
    if (delta < B)return inter;
    inter.hasIntersection = true;
    inter.dis = (B + delta) >= 0 ? (delta - B) : -(delta + B);
    inter.P = ray(inter.dis);
    inter.normal = (inter.P - p).normalize();
    inter.tex = tex;
    return inter;
}

Vec3 Sphere::getCoordMin() const
{
    return p;
}

Vec3 Sphere::getCoordMax() const
{
    return p;
}
