#include "Sphere.h"
#include <cmath>
#include "rd_maths.h"

bool Sphere::intersect(const Ray& ray, SurfaceRecord *pRec) const
{
    Ray objRay = World2Obj(ray);
    Vec3 dif = objRay.orig;
    Real B = dif.dot(objRay.dir);
    Real C = dif.dot(dif) - R * R;
    Real delta = B * B - C;
    if (delta < 0.0) return false;
    delta = std::sqrt(delta);
    if (delta < B) return false;
    Real dis = (B + delta) >= 0 ? (delta - B) : -(delta + B);
    Vec3 localPos = objRay(dis);
    pRec->uv = { localPos.x == 0.0 ? PI_DIV_2 : atan(localPos.y / localPos.x) / , acos(localPos.z / R)};
    pRec->pos = Obj2World(localPos);
    pRec->normal = Obj2World.transNormal(pRec->pos).normalized();
    return true;
}

bool Sphere::intersect(const Ray& ray) const
{
    Ray objRay = World2Obj(ray);
    Vec3 dif = objRay.orig;
    Real B = dif.dot(objRay.dir);
    Real C = dif.dot(dif) - R * R;
    Real delta = B * B - C;
    if (delta < 0.0) return false;
    delta = std::sqrt(delta);
    if (delta < B) return false;
    return true;
}

Patch Sphere::sample(Real *pdf) const
{
    *pdf = PI4_INV;
    Vec3 ret = uniformSampleSphere();
    return { ret * R, ret };
}

Patch Sphere::sample(const Vec3 &ref, Real *pdf) const
{
    // TODO: Implement the sampling here
    return Patch();
}

Patch Sphere::sample(const Vec3 &ref) const
{
    // TODO:
    return Patch();
}
