#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "Ray.hpp"
#include "Vector.h"
#include "Sphere.h"
#include "Scene.hpp"
#include "Materials.h"
const int spp = 16;
const int nx = 1024, ny = 1024;
Scene scene;
int main()
{
    int ScrWid = 4, ScrHeight = 4, ScrZ = -1;
    freopen("output.ppm", "w", stdout);
    std::cout << "P3" << std::endl << nx << " " << ny << std::endl << 255 <<std::endl;
    scene.ObjList.push_back(new Sphere({0.0, 0.0, -1.0}, 0.5, Gold));
    scene.ObjList.push_back(new Sphere({0.0, -100.5, -1.0}, 100.0, Gold));
    scene.ObjList.push_back(new Sphere({1.0, 0.0, -1.0}, 0.5, Silver));
    scene.ObjList.push_back(new Sphere({-0.5, 0.0, -2.0}, 0.5, Copper));
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
            printf("%d %d %d\n", int(color[0] * 255.99), int(color[1] * 255.99), int(color[2] * 255.99));
        }
        std::cerr << static_cast<Real>(ny - j) * 100.0 / ny << "% rendered" << std::endl;
    }
    std::cerr << "Finish Rendering." << std::endl;
}