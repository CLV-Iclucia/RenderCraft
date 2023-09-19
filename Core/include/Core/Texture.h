#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include "core.h"
#include <memory>
/**
 * \brief All textures in RenderCraft are considered as "distribution".
 *        They describe the distribution of properties(roughness, normals, colors, e.g.)
 */

class TextureMapping
{
    public:
        virtual Vec2 map(Real x, Real y) const = 0;
};

/**
 * @tparam T the type of the texture value.
 */
template<typename T>
class Texture
{
    public:
        virtual T eval(const Vec2&) const = 0;
};

template<typename T>
class ConstantTexture : public Texture<T>
{
    public:
        explicit ConstantTexture(const T& _tex) : tex(_tex) {}
        explicit ConstantTexture(T&& _tex) : tex(std::move(_tex)) {}
        T eval(const Vec2&) const override { return tex; }
    private:
        T tex;
};

/**
 * By default, PerlinNoise describes a 2D-texture
 */
template<typename T>
class PerlinNoise : public Texture<T>
{
    private:
        uint m = 0, n = 0;
        T *a = nullptr;
        std::shared_ptr<Texture<T>> texA, texB;
    public:
        PerlinNoise(uint _width, uint _height) : m(_width), n(_height)
        {
            a = new T[m * n];
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    a[i * m + j] = get_random();
        }
        /**
         * Todo: an interpolation method need to be implemented in XMath
         */
        T eval(const Vec2& uv) const override
        {

        }
        ~PerlinNoise() { delete[] a; }
};

template<typename T>
class CheckerBoard : public Texture<T>
{
    private:
        T A, B;
        Real grid_w = 0.0, grid_h = 0.0;
        std::shared_ptr<Texture<T> > texA, texB;
    public:
        CheckerBoard(const Texture<T>& A_, const Texture<T>& B_) : A(A_), B(B_) {}
        T eval(const Vec2& uv) const override
        {
            int _u = floor(uv[0] / grid_w);
            int _v = floor(uv[1] / grid_h);
            Real u = uv[0] - _u * grid_w;
            Real v = uv[1] - _v * grid_h;
            return ((_u ^ _v) & 1) ? A->eval({u, v}) : B->eval({u, v});
        }
};

template<typename T>
class ImageTexture : public Texture<T>
{
    private:

    public:

};

/**
 * \brief it is just a structure to store all the textures used on a material.
 */
struct TextureGroup
{
    std::shared_ptr<Texture<Real> > bump_map;
    std::shared_ptr<Texture<Spectrum> > image_tex;
};
#endif

