//
// Created by creeper on 23-4-12.
//

#ifndef RENDERCRAFT_MEDIUM_H
#define RENDERCRAFT_MEDIUM_H
#include <Core/core.h>
#include <Core/spectrums.h>
#include <Core/texture.h>
#include <Core/phase-function.h>

namespace rdcraft {
class Medium {
  public:
    explicit Medium(std::unique_ptr<PhaseFunction> phase_function)
      : pf(std::move(phase_function)) {
    }
    PhaseFunction* phaseFunction() const {
      return pf.get();
    }
    virtual Spectrum absorptionCoefficient(const Vec3& p) const = 0;
    virtual Spectrum scatterCoefficient(const Vec3& p) const = 0;
    virtual Spectrum extinctionCoefficient(const Vec3& p) const = 0;
    virtual Spectrum getMajorant(const Ray& ray) const = 0;
    virtual ~Medium() = default;

  protected:
    // infact I guess that this can be made a shared_ptr since it is read-only
    std::unique_ptr<PhaseFunction> pf{};
};

struct HomogeneousMedium final : Medium {
  Spectrum sigma_a;
  Spectrum sigma_s;
  HomogeneousMedium(std::unique_ptr<PhaseFunction> phase_function,
                    const Spectrum& sigma_a, const Spectrum& sigma_s)
    : Medium(std::move(phase_function)), sigma_a(sigma_a), sigma_s(sigma_s) {
  }
  Spectrum absorptionCoefficient(const Vec3& p) const override {
    return sigma_a;
  }
  Spectrum scatterCoefficient(const Vec3& p) const override {
    return sigma_s;
  }
  Spectrum extinctionCoefficient(const Vec3& p) const override {
    return sigma_a + sigma_s;
  }
  Spectrum getMajorant(const Ray& ray) const override {
    return sigma_a + sigma_s;
  }
};

struct HeterogeneousMedium final : Medium {
  Spectrum majorant;
  std::unique_ptr<GridVolumeTexture<Spectrum>> density;
  std::unique_ptr<GridVolumeTexture<Spectrum>> albedo;
  Vec3 origin;
  Vec3 size;
  HeterogeneousMedium(std::unique_ptr<PhaseFunction> phase_function,
                      std::unique_ptr<GridVolumeTexture<Spectrum>> density_,
                      std::unique_ptr<GridVolumeTexture<Spectrum>> albedo_,
                      const Vec3& origin = Vec3(0.0), const Vec3& size = Vec3(1.0))
    : Medium(std::move(phase_function)), density(std::move(density_)),
      albedo(std::move(albedo_)), origin(origin), size(size) {
  }
  Spectrum absorptionCoefficient(const Vec3& p) const override {
    return density->eval((p - origin) / size) * (Spectrum(1.0) - albedo->eval((p - origin) / size));
  }
  Spectrum scatterCoefficient(const Vec3& p) const override {
    return density->eval((p - origin) / size) * albedo->eval((p - origin) / size);
  }
  Spectrum extinctionCoefficient(const Vec3& p) const override {
    return density->eval((p - origin) / size);
  }
  Spectrum getMajorant(const Ray& ray) const override {
    // if ray.orig is already inside the medium, then we don't need to check
        // the intersection with the bounding box
    if (ray.orig.x >= origin.x && ray.orig.x <= origin.x + size.x &&
        ray.orig.y >= origin.y && ray.orig.y <= origin.y + size.y &&
        ray.orig.z >= origin.z && ray.orig.z <= origin.z + size.z)
      return density->getMajorant();
    if (!intersectBBox(AABB(origin, origin + size), ray)) return {};
    return density->getMajorant();
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