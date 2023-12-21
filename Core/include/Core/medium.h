//
// Created by creeper on 23-4-12.
//

#ifndef RENDERCRAFT_MEDIUM_H
#define RENDERCRAFT_MEDIUM_H
#include <Core/core.h>
#include <Core/spectrums.h>
#include <texture.h>

namespace rdcraft {
class Medium {
  public:
    virtual ~Medium() = default;
};

struct HomogeneousMedium : public Medium {
  Spectrum sigma_a;
  Spectrum sigma_s;
  HomogeneousMedium(const Spectrum& sigma_a, const Spectrum& sigma_s)
    : sigma_a(sigma_a), sigma_s(sigma_s) {
  }
  Spectrum absorptionCoeffcient(const Vec3& p) const {
    return sigma_a;
  }
  Spectrum scatterCoefficient(const Vec3& p) const {
    return sigma_s;
  }
  Spectrum extinctionCoefficient(const Vec3& p) const {
    return sigma_a + sigma_s;
  }
};

struct HeterogeneousMedium : Medium {
  std::unique_ptr<VolumeTexture<Spectrum>> sigma_s;
  std::unique_ptr<VolumeTexture<Spectrum>> sigma_a;
  Spectrum absorptionCoeffcient(const Vec3& p) const {
    return sigma_a->eval(p);
  }
  Spectrum scatterCoefficient(const Vec3& p) const {
    return sigma_s->eval(p);
  }
  Spectrum extinctionCoefficient(const Vec3& p) const {
    return sigma_a->eval(p) + sigma_s->eval(p);
  }
};

struct MediumInterface {
  int internal_id = -1;
  int external_id = -1;
  MediumInterface(int internal, int external)
    : internal_id(internal), external_id(external) {
  }
};
}
#endif //RENDERCRAFT_MEDIUM_H