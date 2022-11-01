#ifndef RENDERCRAFT_TRIANGLE_H
#define RENDERCRAFT_TRIANGLE_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include "Object.h"
#include "Material.h"
#include "Mesh.h"

class Triangle : public Object
{
    Vec3 v[3];
    Vec2 uv[3];
    Mesh* mesh = nullptr;
    Triangle() = default;
};
#endif