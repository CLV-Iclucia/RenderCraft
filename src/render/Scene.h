#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include <vector>
#include "Object.h"
#include "../../XMath/ext/Vector.h"
#include "Intersection.h"
#include "Ray.h"
#include <algorithm>
#include <memory>
#include "BVH.h"
#include "Materials.h"
#include "Light.h"
#include "EnvMap.h"

struct Scene
{
	std::vector<Object*> ObjList;
	std::vector<Light*> Lights;
    BVH* BVHTree = nullptr;
    std::shared_ptr<EnvMap> envMap;
	const Real PRR = 0.9f;//probability of Russian Roullete
	void load(const std::vector<Object*>& objList);
	void load(Object* obj);
	void intersect(const Ray& ray, Intersection *intsct);
	void init();
	//Regulation: the direction of the ray must be flipped before checking and sampling
	Spectrum cast_ray(Ray& ray);
};
#endif