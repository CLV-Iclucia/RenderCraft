#ifndef RENDERCRAFT_VECTOR_H
#define RENDERCRAFT_VECTOR_H
#include "global.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <initializer_list>
template<typename T, unsigned N>class Vector;
template<typename T, unsigned N>
Vector<T, N> operator*(const T& v, const Vector<T, N>& A);
template<typename T, unsigned N>
Vector<T, N> operator/(const T& v, const Vector<T, N>& A);
template<typename T, unsigned N>
class Vector
{
    friend Vector operator*<T, N>(const T&, const Vector&);
    friend Vector operator/<T, N>(const T&, const Vector&);
    public:
        Vector(){for (int i = 0; i < N; i++)value[i] = 0;}
        Vector(std::initializer_list<T> lst)
        {
            auto it = lst.begin();
            for (int i = 0; i < N; i++, it++)
                value[i] = *it;
        }
        Vector(Vector&& V) noexcept
        {
            for (int i = 0; i < N; i++)
                value[i] = std::move(V.value[i]);
        }
        Vector& operator=(Vector&& V) noexcept
        {
            for (int i = 0; i < N; i++)
                value[i] = std::move(V.value[i]);
            return *this;
        }
        Vector& operator=(const Vector& V)
        {
            if(&V == this) return *this;
            for (int i = 0; i < N; i++)
                value[i] = V.value[i];
            return *this;
        }
        Vector(const Vector& V)
        {
            for (int i = 0; i < N; i++)value[i] = V.value[i];
        }
        explicit Vector(T v)
        {
            for (int i = 0; i < N; i++)value[i] = v;
        }
        Real norm() const
        {
            Real sum = 0.0;
            for (int i = 0; i < N; i++)
                sum += value[i] * value[i];
            return std::sqrt(sum);
        }
        Vector& normalize()
        {
            Real length = this->norm();
            if (length == 0.0)return *this;
            else
            {
                for (int i = 0; i < N; i++)
                    value[i] /= length;
                return *this;
            }
        }
        T dot(const Vector& A) const
        {
            T sum = 0;
            for (int i = 0; i < N; i++)
                sum += this->value[i] * A.value[i];
            return sum;
        }
        Vector reflect(const Vector& A) const //(*this) as a normal, return the reflection of A
        {
            return 2 * dot(A) * (*this) - A;
        }
        T& operator[](int i)
        {
            return value[i];
        }
        const T& operator[](int i) const
        {
            return value[i];
        }
        Vector operator*(const Vector& A) const
        {
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = value[i] * A.value[i];
            return V;
        }
        Vector operator+(const Vector& A) const
        {
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = value[i] + A.value[i];
            return V;
        }
        Vector operator-()
        {
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = -value[i];
            return V;
        }
        Vector operator/(const Vector& A) const
        {
            for (int i = 0; i < N; i++)
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
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = value[i] / A.value[i];
            return V;
        }
        Vector operator-(const Vector& A) const
        {
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = value[i] - A.value[i];
            return V;
        }
        Vector operator/(const T& v) const
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
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = value[i] / v;
            return V;
        }
        Vector operator*(const T& v) const
        {
            Vector V;
            for (int i = 0; i < N; i++)
                V.value[i] = v * value[i];
            return V;
        }
        Vector& operator*=(const Vector& A)
        {
            for (int i = 0; i < N; i++)
                value[i] *= A.value[i];
            return *this;
        }
        Vector& operator+=(const Vector& A)
        {
            for (int i = 0; i < N; i++)
                value[i] += A.value[i];
            return *this;
        }
        Vector& operator/=(const Vector& A)
        {
            for (int i = 0; i < N; i++)
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
        Vector& operator/=(const T& v)
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
            for (int i = 0; i < N; i++)
                value[i] /= v;
            return *this;
        }
        Vector& operator-=(const Vector& A)
        {
            for (int i = 0; i < N; i++)
                value[i] -= A.value[i];
            return *this;
        }
        Vector& operator*=(const T& v)
        {
            for (int i = 0; i < N; i++)
                value[i] *= v;
            return *this;
        }
        ~Vector() = default;
    private:
        T value[N];
};
template<typename T, unsigned N>
Vector<T, N> operator*(const T& v, const Vector<T, N>& A)
{
    return A * v;
}
template<typename T, unsigned N>
Vector<T, N> operator/(const T& v, const Vector<T, N>& A)
{
    return Vector<T, N>(v) / A;
}
using Vec2f = Vector<float, 2u>;
using Vec3f = Vector<float, 3u>;
using Vec4f = Vector<float, 4u>;
using Vec3b = Vector<bool, 3u>;
using Vec2 = Vector<Real, 2u>;
using Vec3 = Vector<Real, 3u>;
using Spectrum = Vector<Real, 3u>;
#endif