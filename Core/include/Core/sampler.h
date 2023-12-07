#ifndef RENDERCRAFT_CORE_SAMPLER_H
#define RENDERCRAFT_CORE_SAMPLER_H

#include <Core/core.h>
#include <random>
namespace rdcraft {

class Sampler {
 public:
  template <int Dim>
  Vector<Real, Dim> sample() {
    Vector<Real, Dim> ret;
    for (int i = 0; i < Dim; i++)
      ret[i] = dis(gen);
    return ret;
  }
 private:
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;
};
}

#endif