//
// Created by creeper on 23-4-11.
//

#ifndef RENDERCRAFT_SPECTRUMS_H
#define RENDERCRAFT_SPECTRUMS_H

#include "types.h"

template<uint N>
class SampledSpectrum
{
    private:
        Real lambda[N];
        Real distrib[N];
    public:

};

using RGBSpectrum = Vec3;
using Spectrum = RGBSpectrum;
#endif //RENDERCRAFT_SPECTRUMS_H
