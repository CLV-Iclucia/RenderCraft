#pragma once
#include "Vector.h"
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