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
        virtual Spectrum L(uint x, uint y) const = 0;
};

class PathTracer : public Integrator
{
    public:
        Spectrum L(const Ray& ray) const override;
};

class VolumePathTracer : public Integrator
{
    public:
        Spectrum L(const Ray& ray) const override;
};

#endif //RENDERCRAFT_INTEGRATOR_H
