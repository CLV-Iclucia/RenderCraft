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
#include "Light.h"
#include "EnvMap.h"

struct Scene
{
	std::vector<Shape*> shapes;
    BVH* BVHTree = nullptr;
    std::shared_ptr<EnvMap> envMap;
	const Real PRR = 0.9f;//probability of Russian Roullete
	void intersect(const Ray& ray, Intersection *intsct);
	void init();

};
#endif