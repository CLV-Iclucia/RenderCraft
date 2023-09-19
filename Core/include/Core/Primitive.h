//
// Created by creeper on 23-4-11.
//

#ifndef RENDERCRAFT_PRIMITIVE_H
#define RENDERCRAFT_PRIMITIVE_H

#include "Light.h"
#include "Medium.h"
#include "Record.h"
#include "Shape.h"
#include "Transform.h"
#include "core.h"
#include <variant>
#include <vector>

class Primitive
{
    public:
        virtual BBox3 getBBox() const = 0;
        virtual Material* getMaterial() const = 0;
        virtual Light* getLight() const = 0;
        virtual bool intersect(const Ray& ray, SurfaceRecord *pRec) const = 0;
        /// mainly used for visibility testing
        virtual bool intersect(const Ray& ray) const = 0;
        virtual bool isLight() const = 0;
};

class GeometryPrimitive : public Primitive
{
    private:
        std::shared_ptr<Shape> shape;
        std::variant<Material*, Light*> surface;
        MediumInterface interface;
    public:
        /**
         * forward the intersection request to the shape
         */
        bool intersect(const Ray& ray, SurfaceRecord *pRec) const override
        {
            bool hasIntersection = shape->intersect(ray, pRec);
            if(hasIntersection) pRec->pr = (Primitive*)this;
            return hasIntersection;
        }
        bool intersect(const Ray& ray) const override { return shape->intersect(ray); }
        bool isLight() const override { return surface.index(); }
        int externalMediumId() const { return interface.external_id; }
        int internalMediumId() const { return interface.internal_id; }
        Material* getMaterial() const override { return std::get<Material*>(surface); }
        Light* getLight() const override { return std::get<Light*>(surface); }
};

class TransformedPrimitive : Primitive
{
    private:
        std::shared_ptr<Primitive> pr;
        Transform World2Pr, Pr2World;
    public:
        bool intersect(const Ray& ray, SurfaceRecord *pRec) const override
        {
            bool hasIntersection = pr->intersect(World2Pr(ray), pRec);
            if(hasIntersection)
            {
                pRec->normal = Pr2World.transNormal(pRec->normal);
                pRec->pos = Pr2World(pRec->pos);
            }
            return hasIntersection;
        }
        bool intersect(const Ray& ray) const override { return pr->intersect(World2Pr(ray)); }
        Material* getMaterial() const override { return pr->getMaterial(); }
        Light* getLight() const override { return pr->getLight(); }
        bool isLight() const override { return pr->isLight(); };
};

class Aggregate : public Primitive
{
    private:
        std::vector<Primitive*> primitives;
        std::shared_ptr<BVH> bvh;
    public:
        Material* getMaterial() const override
        {
            std::cerr << "Aggregate::getMaterial() shouldn't be called" << std::endl;
            exit(-1);
        }
        Light* getLight() const override
        {
            std::cerr << "Aggregate::getLight() shouldn't be called" << std::endl;
            exit(-1);
        }
        bool isLight() const override
        {
            std::cerr << "Aggregate::isLight() shouldn't be called" << std::endl;
            exit(-1);
        }
        bool intersect(const Ray& ray, SurfaceRecord *pRec) const override
        {
            bool hasIntersection = bvh->intersect(ray, pRec);
            if(hasIntersection) pRec->pr = (Primitive*)(this);
            return hasIntersection;
        }
        bool intersect(const Ray& ray) const override { return bvh->intersect(ray); }
        BBox3 getBBox() const override { return bvh->getBBox(); }
};


#endif //RENDERCRAFT_PRIMITIVE_H
