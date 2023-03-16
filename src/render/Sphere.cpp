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

Vec3 Sphere::getLocalCoordMin() const
{
    return { -R, -R, -R };
}

Vec3 Sphere::getLocalCoordMax() const
{
    return { R, R, R };
}

Vec3 Sphere::sampleVisiblePoint(const Vec3& ref, Real *pdf) const
{
    Real dis = ref.norm();
    if(dis <= R)
    {
        *pdf = PI4_INV;
        return uniform_sample_sphere() * R;
    }
    Real local_zMin = 1.0 - R / dis;
    Vec3 ret = uniform_sample_crown(local_zMin) * R;
    *pdf = 1.0 / calcVisibleArea(ref);
    return ret;
}

/**
 * calc the visible area seen from reference point to a Sphere
 * assume that there is no occlusion caused by other objects
 * @param ref the coordinate of reference point
 * @param p the coordinate of the center of the sphere
 * @return the area of the visible region
 */
Real Sphere::calcVisibleArea(const Vec3& ref) const
{
    Real dis = ref.norm();
    if(dis <= R) return PI4 * R * R;
    if(EqualZero(dis)) return PI4 * R * R;
    return PI4 * acos(R / dis) * R * R;
}

Real Sphere::calcArea() const
{
    return PI4 * R * R;
}

Vec3 Sphere::sample(Real *pdf) const
{
    *pdf = PI4_INV;
    return uniform_sample_sphere() * R;
}