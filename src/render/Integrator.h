//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include "types.h"
#include "Scene.h"
#include "Filter.h"
#include "Ray.h"
#include "RenderOptions.h"
#include "Camera.h"
#include "Spectrums.h"

class Integrator
{
    public:
        virtual Spectrum L(const Ray& ray) const = 0;
};

class PathTracer : public Integrator
{
    private:
        std::shared_ptr<Scene> scene;
    public:
        Spectrum L(const Ray& ray) const override;
};

class VolumePathTracer : public Integrator
{
    public:
        Spectrum L(const Ray& ray) const override;
};

#endif //RENDERCRAFT_INTEGRATOR_H
