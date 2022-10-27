#ifndef RENDERCRAFT_MATRIX_H
#define RENDERCRAFT_MATRIX_H
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <initializer_list> 
#include "../XMath/ext/Graphics/MathUtils.h"
#include <cstring>
template<typename T>class Mat3x3;
template<typename T>
Mat3x3<T> operator*(const T& v, const Mat3x3<T>& A);
template<typename T>
class Mat3x3
{
    friend Mat3x3 operator*<T>(const T&, const Mat3x3&);
    public:
        Mat3x3() { for (int i = 0; i < 9; i++) value[i] = 0; }
        Mat3x3(std::initializer_list<T> lst)
        {
            auto it = lst.begin();
            for (int i = 0; i < 9; i++, it++)
                value[i] = *it;
        }
        Mat3x3(const Mat3x3& M)
        {
            for (int i = 0; i < 9; i++)value[i] = M.value[i];
        }
        Mat3x3(const Vec3& A, const Vec3& B, const Vec3& C)
        {
            value[0] = A[0];
            value[1] = A[1];
            value[2] = A[2];
            value[3] = B[0];
            value[4] = B[1];
            value[5] = B[2];
            value[6] = C[0];
            value[7] = C[1];
            value[8] = C[2];
        }
        T& operator()(int i, int j)//instead of [i][j], I use () to make it simpler and (maybe) faster
        {
            return value[i];
        }
        const T& operator()(int i, int j) const
        {
            return value[i];
        }
        Vec3 operator*(const Vec3& A) const
        {
            Vec3 V;
            V[0] = value[0] * A[0] + value[3] * A[1] + value[6] * A[2];
            V[1] = value[1] * A[0] + value[4] * A[1] + value[7] * A[2];
            V[2] = value[2] * A[0] + value[5] * A[1] + value[8] * A[2];
            return V;
        }
        Mat3x3 operator*(const Mat3x3& A) const
        {
            Mat3x3 M;
            M.value[0] = value[0] * A.value[0] + value[3] * A.value[1] + value[6] * A.value[2];
            M.value[3] = value[0] * A.value[3] + value[3] * A.value[4] + value[6] * A.value[5];
            M.value[6] = value[0] * A.value[6] + value[3] * A.value[7] + value[6] * A.value[8];
            M.value[1] = value[1] * A.value[0] + value[4] * A.value[1] + value[7] * A.value[2];
            M.value[4] = value[1] * A.value[3] + value[4] * A.value[4] + value[7] * A.value[5];
            M.value[7] = value[1] * A.value[6] + value[4] * A.value[7] + value[7] * A.value[8];
            M.value[2] = value[2] * A.value[0] + value[5] * A.value[1] + value[8] * A.value[2];
            M.value[5] = value[2] * A.value[3] + value[5] * A.value[4] + value[8] * A.value[5];
            M.value[8] = value[2] * A.value[6] + value[5] * A.value[7] + value[8] * A.value[8];
            return M;
        }
        Mat3x3 operator+(const Mat3x3& A) const
        {
            Mat3x3 M;
            for (int i = 0; i < 9; i++)
                M.value[i] = value[i] + A.value[i];
            return M;
        }
        Mat3x3 operator/(const Mat3x3& A) const
        {
            for (int i = 0; i < 9; i++)
            {
                try
                {
                    if (A.value[i] == 0)throw std::runtime_error("Division by zero!");
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    exit(-1);
                }
            }
            Mat3x3 M;
            for (int i = 0; i < 9; i++)
                M.value[i] = value[i] / A.value[i];
            return M;
        }
        Mat3x3 operator-(const Mat3x3& A) const
        {
            Mat3x3 M;
            for (int i = 0; i < 9; i++)
                M.value[i] = value[i] - A.value[i];
            return M;
        }
        Mat3x3 operator/(const T& v) const
        {
            try
            {
                if (v == 0)throw std::runtime_error("Division by zero!");
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                exit(-1);
            }
            Mat3x3 M;
            for (int i = 0; i < 9; i++)
                M.value[i] = value[i] / v;
            return M;
        }
        Mat3x3 operator*(const T& v) const
        {
            Mat3x3 M;
            for (int i = 0; i < 9; i++)
                M.value[i] = v * value[i];
            return M;
        }
        Mat3x3& operator*=(const Mat3x3& A)
        {
            for (int i = 0; i < 9; i++)
                value[i] *= A.value[i];
            return *this;
        }
        Mat3x3& operator+=(const Mat3x3& A)
        {
            for (int i = 0; i < 9; i++)
                value[i] += A.value[i];
            return *this;
        }
        Mat3x3& operator/=(const Mat3x3& A)
        {
            for (int i = 0; i < 9; i++)
            {
                try
                {
                    if (A.value[i] == 0)throw std::runtime_error("Division by zero!");
                    else value[i] /= A.value[i];
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    exit(-1);
                }
            }
            return *this;
        }
        Mat3x3& operator/=(const T& v)
        {
            try
            {
                if (v == 0)throw std::runtime_error("Division by zero!");
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                exit(-1);
            }
            for (int i = 0; i < 9; i++)
                value[i] /= v;
            return *this;
        }
        Mat3x3& operator-=(const Mat3x3& A)
        {
            for (int i = 0; i < 9; i++)
                value[i] -= A.value[i];
            return *this;
        }
        Mat3x3& operator*=(const T& v)
        {
            for (int i = 0; i < 9; i++)
                value[i] *= v;
            return *this;
        }
        ~Mat3x3() = default;
    private:
        T value[9];//Column-Major Order
};
template<typename T>
Mat3x3<T> operator*(const T& v, const Mat3x3<T>& A)
{
    return A * v;
}
using Mat3f = Mat3x3<Real>;
#endif