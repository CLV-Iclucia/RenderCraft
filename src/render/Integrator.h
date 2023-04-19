//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include <memory>
#include "types.h"
#include "Filter.h"
#include "Ray.h"
#include "RenderOptions.h"
#include "Camera.h"
#include "Spectrums.h"
#include "Scene.h"

class Integrator
{
    protected:
        std::shared_ptr<Scene> scene = nullptr;
    public:
        virtual Spectrum L(const Ray& ray) const = 0;
};

class PathTracer final: public Integrator
{
    private:
        Spectrum nextEventEst() const;
    public:
        Spectrum L(const Ray& ray) const override;
};

class VolumePathTracer : public Integrator
{
    public:
        Spectrum L(const Ray& ray) const override;
};

#endif //RENDERCRAFT_INTEGRATOR_H
