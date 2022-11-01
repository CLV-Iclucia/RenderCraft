#ifndef RENDERCRAFT_INTERSECTION_H
#define RENDERCRAFT_INTERSECTION_H
#include <utility>

#include "../XMath/ext/Graphics/MathUtils.h"
#include "Material.h"
#include "Texture.h"
#include "Ray.hpp"
struct Intersection
{
	bool hasIntersection = false;
	Vec3 P, normal;///< coord of intersection point and corresponding normal
    Vec2 uv;///< tex coord of the intersection point
	Real dis = 0.0;
    Vec3 dpdu, dpdv;
    Vec3 dndu, dndv;
	Material* mat = nullptr;
	Intersection() = default;
	Intersection(bool _hasIntersection, Vec3 P_, Vec3 _normal, Vec2 uv_, Real _dis,
                 Vec3 dpdu_, Vec3 dpdv_, Vec3 dndu_, Vec3 dndv_, Material* _mat) :
		hasIntersection(_hasIntersection), P(std::move(P_)), normal(std::move(_normal)),
                        uv(std::move(uv_)), dis(_dis), dpdu(std::move(dpdu_)),
                        dpdv(std::move(dpdv_)), dndu(std::move(dndu_)), dndv(std::move(dndv_)),
                        mat(_mat) {}
    /**
     * \brief calculate the ray differential, using the method in pbrt-v3
     */
    void CalcRayDifferential(const RayDiff&) const;
};
#endif