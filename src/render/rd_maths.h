//
// Created by creeper on 23-4-17.
//

#ifndef RENDERCRAFT_RD_MATHS_H
#define RENDERCRAFT_RD_MATHS_H
#include "types.h"
#include <mx3d_utils.h>

using mx3d::isZero;
using mx3d::isEqual;
const Real PI_DIV_2 = 1.5707963267948966;
const Real PI4_INV = 0.07957747154594767;
const Real PI2 = 6.283185307179586;

inline Real distance(const Vec3& A, const Vec3& B) { return (A - B).norm(); }
inline Real distSqr(const Vec3& A, const Vec3& B) { return (A - B).dot(A - B); }
Real get_random()
{

}

Vec3 uniformSampleSphere()
{

}

Mat3 constructFrame(const Vec3& N)
{

}

Real geometry(const Patch& A, const Patch& B)
{ return std::max(-A.n.dot(B.n), 0.0) / distSqr(A.p, B.p); }

#endif //RENDERCRAFT_RD_MATHS_H
