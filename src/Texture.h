#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include "../XMath/ext/Graphics/MathUtils.h"
#include <memory>
/**
 * @tparam T the type of the texture value
 *
 */
template<typename T>
class Texture
{
    public:
        virtual T eval(Real u, Real v) const = 0;
};

template<typename T>
class UniformTexture : public Texture<T>
{
    public:
        T eval(Real u, Real v) const override { return tex; }
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
        T eval(Real u, Real v) const override
        {

        }
        ~PerlinNoise() { delete[] a; }
};

template<typename T>
class CheckBoard : public Texture<T>
{
    private:
        T A, B;
        Real grid_w = 0.0, grid_h = 0.0;
        std::shared_ptr<Texture<T> > texA, texB;
    public:
        CheckBoard(const Texture<T>& A_, const Texture<T>& B_) : A(A_), B(B_) {}
        T eval(Real u, Real v) const override
        {
            int _u = floor(u / grid_w);
            int _v = floor(u / grid_h);
            u -= _u * grid_w;
            v -= _v * grid_h;
            return ((_u ^ _v) & 1) ? A->eval(u, v) : B->eval(u, v);
        }
};

template<typename T>
class ImageTexture : public Texture<T>
{
    private:

    public:

};
#endif

