#ifndef RENDERCRAFT_CORE_SAMPLER_H
#define RENDERCRAFT_CORE_SAMPLER_H

#include <Core/core.h>
#include <random>

namespace rdcraft {
class Sampler {
  public:
    Real get() {
      return dis(gen);
    }
    template <int Dim>
    Vector<Real, Dim> sample() {
      Vector<Real, Dim> ret;
      for (int i = 0; i < Dim; i++)
        ret[i] = dis(gen);
      return ret;
    }
    Real sample() {
      return dis(gen);
    }

  private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
};

// [l, r)
inline int randomInt(Sampler& sampler, int l, int r) {
  Real u = sampler.sample();
  return static_cast<int>(std::floor(u * (r - l))) + l;
}

inline Real randomReal(Sampler& sampler, Real l, Real r) {
  Real u = sampler.sample();
  return u * (r - l) + l;
}

struct DiscreteDistribution {
  DiscreteDistribution() = default;
  explicit DiscreteDistribution(int n)
    : probabilities(n) {
  }
  explicit DiscreteDistribution(const std::vector<Real>& weights) {
    buildFromWeights(weights);
  }
  void buildFromWeights(const std::vector<Real>& weights) {
    probabilities = weights;
    auto n = probabilities.size();
    alias.resize(n);
    std::vector<Real> scaledProbabilities = probabilities;
    for (int i = 0; i < n; i++)
      scaledProbabilities[i] *= n;
    std::vector<int> small, large;
    for (int i = 0; i < n; ++i) {
      if (scaledProbabilities[i] < 1.0)
        small.push_back(i);
      else
        large.push_back(i);
    }

    while (!small.empty() && !large.empty()) {
      int less = small.back();
      small.pop_back();
      int more = large.back();
      large.pop_back();

      alias[less] = more;
      scaledProbabilities[more] =
          scaledProbabilities[more] + scaledProbabilities[less] - 1.0;

      if (scaledProbabilities[more] < 1.0)
        small.push_back(more);
      else
        large.push_back(more);
    }

    while (!large.empty()) {
      int curr = large.back();
      large.pop_back();
      scaledProbabilities[curr] = 1.0;
    }

    while (!small.empty()) {
      int curr = small.back();
      small.pop_back();
      scaledProbabilities[curr] = 1.0;
    }
  }

  struct SampleRecord {
    int idx;
    Real pdf;
  };
  SampleRecord sample(Sampler& sampler) const {
    int idx = randomInt(sampler, 0, probabilities.size());
    Real pdf = probabilities[idx] * probabilities.size();
    if (sampler.sample() < pdf)
      return {idx, pdf};
    return {alias[idx], pdf};
  }
  Real prob(int idx) const {
    return probabilities[idx] * probabilities.size();
  }
  std::vector<Real> probabilities;
  std::vector<int> alias;
};

inline Vec3 uniformSampleSphere(Sampler& sampler) {
  const Real phi = sampler.sample() * PI2;
  const Real cosTheta = 1.0 - 2.0 * sampler.sample();
  const Real sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
  return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

//uniformly sample a point on a disk x^2+y^2=1
inline Vec2 uniformSampleDisk(Sampler& sampler) {
  const Real phi = sampler.sample() * PI2;
  const Real R = std::sqrt(sampler.sample());
  return Vec2(R * std::cos(phi), R * std::sin(phi));
}

inline Vec3 cosWeightedSampleHemisphere(Sampler& sampler) {
  const Vec2 p = uniformSampleDisk(sampler);
  return Vec3(p.x, p.y, std::sqrt(1 - p.x * p.x - p.y * p.y));
}

}

#endif