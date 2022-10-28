#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include "../XMath/ext/Graphics/MathUtils.h"

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

/**
 * By default, PerlinNoise describes a 2D-texture
 */
template<typename T>
class PerlinNoise : Texture<T>
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
class CheckBoard : Texture<T>
{
    private:
        T A, B;
        Real grid_w = 0.0, grid_h = 0.0;
    public:
        CheckBoard(const T& _A, const T& _B) : A(A), B(_B) {}
        T eval(Real u, Real v) const override
        {
            int _u = floor(u / grid_w);
            int _v = floor(u / grid_h);
            return ((_u ^ _v) & 1) ? A : B;
        }
};

template<typename T>
class ImageTexture : Texture<T>
{
    private:

    public:

};
#endif

