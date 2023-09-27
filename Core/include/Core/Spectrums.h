//
// Created by creeper on 23-4-11.
//

#ifndef RENDERCRAFT_SPECTRUMS_H
#define RENDERCRAFT_SPECTRUMS_H

#include <Core/core.h>
#include <vector>
namespace rdcraft {
static Mat3 XYZtoRGB = Mat3(3.240479, -1.537150, -0.498535, -0.969256, 1.875992,
                            0.041556, 0.055648, -0.204043, 1.057311);
static Mat3 RGBtoXYZ = Mat3(0.412453, 0.357580, 0.180423, 0.212671, 0.715160,
                            0.072169, 0.019334, 0.119193, 0.950227);

// these are directly copied from Tzu-Mao Li's lajolla public
// curve proposed by Wyman et al.: https://jcgt.org/published/0002/02/01/
inline Real xFit_1931(Real wavelength) {
    Real t1 = (wavelength - Real(442.0)) * ((wavelength < Real(442.0)) ? Real(0.0624) : Real(0.0374));
    Real t2 = (wavelength - Real(599.8)) * ((wavelength < Real(599.8)) ? Real(0.0264) : Real(0.0323));
    Real t3 = (wavelength - Real(501.1)) * ((wavelength < Real(501.1)) ? Real(0.0490) : Real(0.0382));
    return Real(0.362) * exp(-Real(0.5) * t1 * t1) + 
           Real(1.056) * exp(-Real(0.5) * t2 * t2) -
           Real(0.065) * exp(-Real(0.5) * t3 * t3);
}
inline Real yFit_1931(Real wavelength) {
    Real t1 = (wavelength - Real(568.8)) * ((wavelength < Real(568.8)) ? Real(0.0213) : Real(0.0247));
    Real t2 = (wavelength - Real(530.9)) * ((wavelength < Real(530.9)) ? Real(0.0613) : Real(0.0322));
    return Real(0.821) * exp(-Real(0.5) * t1 * t1) +
           Real(0.286) * exp(-Real(0.5) * t2 * t2);
}
inline Real zFit_1931(Real wavelength) {
    Real t1 = (wavelength - Real(437.0)) * ((wavelength < Real(437.0)) ? Real(0.0845) : Real(0.0278));
    Real t2 = (wavelength - Real(459.0)) * ((wavelength < Real(459.0)) ? Real(0.0385) : Real(0.0725));
    return Real(1.217) * exp(-Real(0.5) * t1 * t1) +
           Real(0.681) * exp(-Real(0.5) * t2 * t2);
}

inline Vec3 XYZCoeff(Real wavelength) {
  return Vec3{xFit_1931(wavelength), yFit_1931(wavelength),
              zFit_1931(wavelength)};
}
template <uint N> class SampledSpectrum {
private:
  Real lambda[N];
  Real distrib[N];

public:
  Vec3 toXYZ() const {
    Real last_lambda = 0;
    Vec3 xyz;
    for (int i = 0; i < N; i++) {
      auto coeff = XYZCoeff(lambda[i]);
      xyz += (lambda[i] - last_lambda) * coeff * distrib[i];
      last_lambda = lambda[i];
    }
    return xyz;
  }
  Vec3 toRGB() const {
    return XYZtoRGB * toXYZ();
  }
};


class AdaptiveSpectrum {
private:
  std::vector<std::tuple<int, Real>> distrib;

public:
  // implement a constructor that takes a vector of tuples
  // each tuple contains a wavelength and a value
  AdaptiveSpectrum(const std::vector<std::tuple<int, Real>>& distrib)
      : distrib(distrib) {}
  Vec3 toXYZ() const {
    Real last_lambda = 0;
    Vec3 xyz;
    for (int i = 0; i < distrib.size(); i++) {
      auto [lambda, value] = distrib[i];
      auto coeff = XYZCoeff(lambda);
      xyz += (lambda - last_lambda) * coeff * value;
      last_lambda = lambda;
    }
    return xyz;
  }
  Vec3 toRGB() const {
    return XYZtoRGB * toXYZ();
  }
};

inline Vec3 makeZeroSpectrum() { return Vec3(0.0); }
using RGBSpectrum = Vec3;
using Spectrum = RGBSpectrum;
inline Spectrum toXYZ(const std::vector<std::tuple<int, Real>>& spec) {
  // almost the same as the method toXYZ() in AdaptiveSpectrum 
  Real last_lambda = 0;
  Vec3 xyz;
  for (int i = 0; i < spec.size(); i++) {
    auto [lambda, value] = spec[i];
    auto coeff = XYZCoeff(lambda);
    xyz += (lambda - last_lambda) * coeff * value;
    last_lambda = lambda;
  }
}
} // namespace rdcraft
#endif // RENDERCRAFT_SPECTRUMS_H
