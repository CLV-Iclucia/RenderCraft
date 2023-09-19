//
// Created by creeper on 22-10-28.
//
#include <cstdio>
#include <cmath>
#include <fstream>
#include "Scene.h"
#include "Integrator.h"

void Scene::render()
{
    std::fstream result(opt.savingPath, std::ios_base::out);
    result << "P3" << std::endl << camera.nx << " " << camera.ny << std::endl << 255 << std::endl;
    for(int j = static_cast<int>(camera.ny) - 1; j >= 0; j--)
    {
        for(int i = 0; i < camera.nx; i++)
        {
            Vec3 radiance;
            Ray ray;
            for (int k = 0; k < opt.spp; k++)
            {
                Vec2 offset = filter->sample();
                const Real rx = (i + offset[0]) * opt.scrWid / camera.nx - opt.scrWid * 0.5;
                const Real ry = (j + offset[1]) * opt.scrHeight / camera.ny - opt.scrHeight * 0.5;
                camera.castRay({rx, ry, -opt.scrZ}, &ray);
                radiance += integrator->render(ray) / filter->pdfSample(offset[0], offset[1]);
            }
            radiance /= static_cast<Real>(opt.spp);
            if (radiance[0] >= 1.0) radiance[0] = 1.0;
            if (radiance[1] >= 1.0) radiance[1] = 1.0;
            if (radiance[2] >= 1.0) radiance[2] = 1.0;
            radiance[0] = std::sqrt(radiance[0]);
            radiance[1] = std::sqrt(radiance[1]);
            radiance[2] = std::sqrt(radiance[2]);
            result << int(radiance[0] * 255.99) << " " << int(radiance[1] * 255.99) << " " << int(radiance[2] * 255.99) << std::endl;
        }
        if(opt.enableDisplayProcess)
        {
            fflush(stdout);
            printf("%.2lf %% rendered\n", static_cast<Real>(camera.ny - j) * 100.0 / camera.ny);
        }
    }
    fflush(stdout);
    printf("Finish rendering\n");
}
