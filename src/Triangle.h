#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Object.h"
#include "Material.h"
class Triangle : public Object
{
    Vec3 v[3];
    Material* m;
    Triangle()
    {
        for(int i = 0; i < 3; i++)
            v[i] = Vec3();
        m = nullptr;
    }
    
};
#endif