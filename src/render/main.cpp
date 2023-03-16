#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "Ray.h"
#include "Sphere.h"
#include "Scene.h"
Scene scene;
int main()
{
    int ScrWid = 4, ScrHeight = 4, ScrZ = -1;
    std::fstream fout("./output.ppm", std::ios_base::out);
    fout << "P3" << std::endl << nx << " " << ny << std::endl << 255 <<std::endl;
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
                color += scene.cast_ray(ray);
            }
            color /= Real(spp);
            if (color[0] >= 1.0)color[0] = 1.0;
            if (color[1] >= 1.0)color[1] = 1.0;
            if (color[2] >= 1.0)color[2] = 1.0;
            color[0] = std::sqrt(color[0]);
            color[1] = std::sqrt(
            fout << int(color[0] * 255.99) << " " << int(color[1] * 255.99) << " " << int(color[2] * 255.99) << std::endl;
        }
        fflush(stdout);
        printf("%.2lf %% rendered\n", static_cast<Real>(ny - j) * 100.0 / ny);
    }
    fflush(stdout);
    printf("Finish rendering\n");
}