#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "Ray.hpp"
#include "Sphere.h"
#include "Scene.h"
#include "Materials.h"
const int spp = 256;
const int nx = 1024, ny = 1024;
Scene scene;
int main()
{
    int ScrWid = 4, ScrHeight = 4, ScrZ = -1;
    std::fstream fout("./output.ppm", std::ios_base::out);
    fout << "P3" << std::endl << nx << " " << ny << std::endl << 255 <<std::endl;
    scene.load(new Sphere({0.0, 0.0, -1.0}, 0.5, Gold));
    scene.load(new Sphere({0.0, -100.5, -1.0}, 100.0, Gold));
    scene.load(new Sphere({1.0, 0.0, -1.0}, 0.5, Silver));
    scene.load(new Sphere({-0.5, 0.0, -2.0}, 0.5, Copper));
    scene.load(new Sphere({-1.5, 0.2, -1.3}, 0.2, Silver));
    scene.load(new Sphere({-2.0, 0.6, 1.0}, 0.1, Gold));
    scene.envMap = std::static_pointer_cast<EnvMap>(std::make_shared<Sky>(0.6, 0.0, 0.0, 0.6, 0.3, 0.3));
    for(int j = ny-1; j >= 0; j--)
    {
        for(int i = 0; i < nx; i++)
        {
            Vec3 color;
            for (int k = 0; k < spp; k++)
            {
                const Real rx = (i + get_random()) * ScrWid / nx - ScrWid * 0.5;
                const Real ry = (j + get_random()) * ScrHeight / ny - ScrHeight * 0.5;
                Vec3 dir({rx, ry, -1});
                dir.normalize();
                Ray ray({0, 0, 0}, dir);
                color += scene.cast_ray(ray, 0);
            }
            color /= Real(spp);
            if (color[0] >= 1.0)color[0] = 1.0;
            if (color[1] >= 1.0)color[1] = 1.0;
            if (color[2] >= 1.0)color[2] = 1.0;
            color[0] = std::sqrt(color[0]);
            color[1] = std::sqrt(color[1]);
            color[2] = std::sqrt(color[2]);
            fout << int(color[0] * 255.99) << " " << int(color[1] * 255.99) << " " << int(color[2] * 255.99) << std::endl;
        }
        fflush(stdout);
        printf("%.2lf %% rendered\n", static_cast<Real>(ny - j) * 100.0 / ny);
    }
    fflush(stdout);
    printf("Finish rendering\n");
}