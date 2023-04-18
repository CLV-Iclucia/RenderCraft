//
// Created by creeper on 23-3-17.
//

#ifndef RENDERCRAFT_SURFACE_H
#define RENDERCRAFT_SURFACE_H

#include "Transform.h"
#include "Shape.h"

class Surface
{
    private:
        std::shared_ptr<Shape> pShape;
        Transform transform;
    public:
        std::shared_ptr<Shape> shape() { return pShape; }
        void intersect(const Ray& ray, SurfaceInteraction* intsct) const
        {
            pShape->intersect(ray, intsct);
        }
};

inline bool surfacePtrCmp(Surface* A, Surface* B)
{
    return A->shape()->getX() < B->shape()->getX();
}

#endif //RENDERCRAFT_SURFACE_H
