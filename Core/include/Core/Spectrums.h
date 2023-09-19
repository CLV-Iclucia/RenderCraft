//
// Created by creeper on 23-4-11.
//

#ifndef RENDERCRAFT_SPECTRUMS_H
#define RENDERCRAFT_SPECTRUMS_H

#include "core.h"

template<uint N>
class SampledSpectrum
{
    private:
        Real lambda[N];
        Real distrib[N];
    public:
        Vec3 toXYZ() const
        {

        }
        Vec3 toRGB() const
        {

        }
};

inline Vec3 XYZ2RGB(const Vec3& spec)
{

}

Vec3 makeZeroSpectrum() { return Vec3(0.0); }

using RGBSpectrum = Vec3;
using Spectrum = RGBSpectrum;
#endif //RENDERCRAFT_SPECTRUMS_H
