//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include "../../XMath/ext/Vector.h"
#include "types.h"
#include "Scene.h"
#include "Filter.h"
#include "Ray.h"
#include "RenderOptions.h"
#include "Camera.h"

class Integrator
{
    public:
        virtual Spectrum render(uint x, uint y) const = 0;
};

class PathTracer : public Integrator
{
    private:
        std::shared_ptr<Filter> filter;
        Scene scene;
        RenderOptions opt;
        Camera camera;
        // Regulation: the direction of the ray must be flipped before checking and sampling
        Spectrum castRay(const Ray& ray) const;
    public:
        Spectrum render(uint x, uint y) const override;
};


#endif //RENDERCRAFT_INTEGRATOR_H
