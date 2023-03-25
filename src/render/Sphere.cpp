#include "Sphere.h"
#include <cmath>
#include <cassert>

void Sphere::intersect(const Ray& ray, Intersection *intsct) const
{
    Vec3 dif = ray.orig;
    Real B = dif.dot(ray.dir);
    Real C = dif.dot(dif) - R * R;
    Real delta = B * B - C;
    if (delta < 0.0) return ;
    delta = std::sqrt(delta);
    if (delta < B) return ;
    intsct->uv = {};
    intsct->hasIntersection = true;
    intsct->dis = (B + delta) >= 0 ? (delta - B) : -(delta + B);
    intsct->P = ray(intsct->dis);
    intsct->normal = (intsct->P).normalized();
}

Vec3 Sphere::sample(Real *pdf) const
{
    *pdf = PI4_INV;
    return uniform_sample_sphere() * R;
}