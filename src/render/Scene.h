#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include <vector>
#include "Record.h"
#include "Ray.h"
#include <algorithm>
#include <memory>
#include "BVH.h"
#include "Light.h"
#include "EnvMap.h"
#include "Filter.h"
#include "RenderOptions.h"
#include "Camera.h"
#include "Primitive.h"

class Integrator;
struct Scene
{
    std::shared_ptr<Filter> filter;
    RenderOptions opt;
    Camera camera;
    Aggregate* pr;
    BVH* BVHTree = nullptr;
    std::shared_ptr<EnvMap> envMap = nullptr;
    std::shared_ptr<Integrator> integrator;
    void render(); ///< compute the radiance of pixel (x, y)
};
#endif