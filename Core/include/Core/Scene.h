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
    Camera camera;
    Aggregate* pr;
    BVH* BVHTree = nullptr;
    std::shared_ptr<EnvMap> envMap = nullptr;
    std::shared_ptr<Integrator> integrator;
    std::vector<AreaLight*> areaLights;
    void render(); ///< compute the radiance of pixel (x, y)
    Light* sampleLight(Real* pdf) const
    {
        // TODO: implement this!
    }
    bool testVisibility(const Vec3& A, const Vec3& B) const
    {
        Ray test_ray(A, B - A);
        return pr->intersect(test_ray);
    }
    Real pdfSampleLight() const
    {

    }
};
#endif