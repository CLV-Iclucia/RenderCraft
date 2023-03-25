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
#include "Filter.h"
#include "RenderOptions.h"
#include "Camera.h"
#include "Integrator.h"

struct Scene
{
    std::shared_ptr<Filter> filter;
    RenderOptions opt;
    Camera camera;
	std::vector<Surface*> surfaces;
    BVH* BVHTree = nullptr;
    std::shared_ptr<EnvMap> envMap;
    std::shared_ptr<Integrator> integrator;
	const Real PRR = 0.9f;//probability of Russian Roullete
	void intersect(const Ray& ray, Intersection *intsct) const;
	void init();
    void render(); ///< compute the radiance of pixel (x, y)
};
#endif