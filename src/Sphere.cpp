#include "Sphere.h"
#include <cmath>
Intersection Sphere::intersect(const Ray& ray, const Vec3& world_pos) const
{
    Vec3 dif = ray.orig - world_pos;
    Real B = dif.dot(ray.dir);
    Real C = dif.dot(dif) - R * R;
    Real delta = B * B - C;
    Intersection inter;
    if (delta < 0.0) return inter;
    delta = std::sqrt(delta);
    if (delta < B) return inter;
    inter.hasIntersection = true;
    inter.dis = (B + delta) >= 0 ? (delta - B) : -(delta + B);
    inter.P = ray(inter.dis);
    inter.normal = (inter.P - world_pos).normalize();
    return inter;
}

Vec3 Sphere::getLocalCoordMin() const
{
    return { -R, -R, -R };
}

Vec3 Sphere::getLocalCoordMax() const
{
    return { R, R, R };
}
