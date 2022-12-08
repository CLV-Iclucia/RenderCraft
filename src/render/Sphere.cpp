#include "Sphere.h"
#include <cmath>
using xmath::EqualZero;
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

Vec3 Sphere::sampleVisiblePoint(const Vec3& ref, const Vec3& p, Real& pdf) const
{
    Real dis = (ref - p).norm();
    if(dis <= R)
    {
        pdf = PI4_INV;
        return uniform_sample_sphere() * R + p;
    }
    Vec3 world_z = (ref - p).normalize();
    Mat3 to_world = construct_frame(world_z);
    Real local_zMin = 1.0 - R / dis;
    Vec3 ret = uniform_sample_crown(local_zMin);
    local_to_world(to_world, ret);
    ret = ret * R + p;
    pdf = 1.0 / calcVisibleArea(ref, p);
    return ret;
}

/**
 * calc the visible area seen from reference point to a Sphere
 * assume that there is no occlusion caused by other objects
 * @param ref the coordinate of reference point
 * @param p the coordinate of the center of the sphere
 * @return the area of the visible region
 */
Real Sphere::calcVisibleArea(const Vec3& ref, const Vec3& p) const
{
    Real dis = dist(ref, p);
    if(dis <= R) return PI4 * R * R;
    if(EqualZero(dis)) return PI4 * R * R;
    return PI4 * acos(R / dis) * R * R;
}

Real Sphere::calcArea() const
{
    return 0;
}

Vec3 Sphere::sample(const Vec3 &p) const
{
    return ext::Vec3();
}

Vec3 Sphere::sample() const
{
    return ext::Vec3();
}
