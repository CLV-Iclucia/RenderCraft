#ifndef RENDERCRAFT_SCENE_HPP
#define RENDERCRAFT_SCENE_HPP
#include <vector>
#include "Object.h"
#include "../XMath/MyMath.h"
#include "Intersection.h"
#include "Ray.hpp"
#include <algorithm>
#include "BVH.h"
#include "Materials.h"
struct Scene
{
	//I give two types of cast_ray function here, one is more flexible
	//while the other one is easier to implement, and is also faster
	std::vector<Object*> ObjList;
	BVH* BVHTree = nullptr;
	Vec3 BackgroundColor = { 0.3f, 0.7f, 1.0f };
	const Real PRR = 0.9f;//probability of Russian Roullete
	void load(const std::vector<Object*>& objList)
	{
		for (auto obj : objList)
			ObjList.push_back(obj);
	}
	void load(Object* obj)
	{
		ObjList.push_back(obj);
	}
	Intersection intersect(const Ray& ray)
	{
		Intersection ret;
		for (auto obj : ObjList)
		{
			Intersection inter = obj->intersect(ray);
			if (inter.hasIntersection && (!ret.hasIntersection || ret.dis >= inter.dis))
				ret = inter;
		}
		return ret;
	}
	void init()
	{
		std::sort(ObjList.begin(), ObjList.end(), ObjectPtrCmp);
		BVHTree = new BVH(ObjList);
	}
	//Regulation: the direction of the ray must be flipped before checking and sampling
	Vec3 cast_ray(const Ray& ray, int depth)
	{
		Intersection inter = intersect(ray);
		Vec3 wo = ray.dir;
		if (!inter.hasIntersection)
		{
			if (!depth)
			{
				Real lerp_ratio = 0.5 * wo[1] + 0.5;
				return lerp(BackgroundColor, { 1.0f, 1.0f, 1.0f }, lerp_ratio);
			}
			else return Vec3();
		}
		Material* mat = inter.mat;
		const Vec3& N = inter.normal;
		wo = -wo;
		if (checkInside(wo, N) && !mat->translucent) return Vec3();
		const Vec3& P = inter.P;
		Vec3 ret;
		Real pdf_inv;
		Vec3 wi = mat->sample(N, wo, pdf_inv);
		Vec3 perturb_P = checkInside(wi, N) ? P - N * EPS : P + N * EPS;
		Intersection dir_inter = intersect(Ray(perturb_P, wi));
		if (!dir_inter.hasIntersection)//calc the direct radiance from the environment/sky
		{
			Real lerp_ratio = 0.5 * wi[1] + 0.5;
			const Vec3 dir_rad = lerp(BackgroundColor, { 1.0f, 1.0f, 1.0f }, lerp_ratio);
			const Real cosThetaI = std::abs(N.dot(wi));
			ret += pdf_inv * mat->BxDF(wi, wo, N) * cosThetaI * dir_rad;//Using Monte Carlo Integration
		}
		if (get_random() > PRR) return ret;//sample at a probability of PRR and terminate at a probability of 1-PRR
		wi = mat->sample(N, wo, pdf_inv);//calculate the indirect radiance
		const Real cosThetaI = std::abs(N.dot(wi));
		perturb_P = checkInside(wi, N) ? P - N * EPS : P + N * EPS;
		return ret + mat->BxDF(wi, wo, N) * pdf_inv * cosThetaI * cast_ray(Ray(perturb_P, wi), depth + 1) / PRR;
	}
};
#endif