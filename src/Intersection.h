#ifndef RENDERCRAFT_INTERSECTION_H
#define RENDERCRAFT_INTERSECTION_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Material.h"
#include "Texture.h"
struct Intersection
{
	bool hasIntersection = false;
	Vec3 P, normal;
	Real dis = 0.0;
	Material* mat = nullptr;
	Intersection() = default;
	Intersection(bool _hasIntersection, Vec3 P_, Vec3 _normal, Real _dis, Material* _mat) :
		hasIntersection(_hasIntersection), P(std::move(P_)), normal(std::move(_normal)), dis(_dis), mat(_mat) {}
};
#endif