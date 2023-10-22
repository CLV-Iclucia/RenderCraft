//
// Created by creeper on 23-4-12.
//

#ifndef RENDERCRAFT_MEDIUM_H
#define RENDERCRAFT_MEDIUM_H
#include "Core/core.h"
#include <Core/Spectrums.h>
namespace rdcraft {
struct Medium {
  AABB bound;
  Vec3i resolution;
  std::vector<Spectrum> sigma_a; // absorption coefficient
  std::vector<Spectrum> sigma_s; // scattering coefficient
  Medium(const AABB &bound, const Vec3i &resolution) : bound(bound), resolution(resolution) {}
  Spectrum getAbsorption(const Vec3 &p) const {
    // calc the absorption coefficient at point p
    // use trilinear interpolation
    Vec3 t = (p - lo(bound)) / (hi(bound) - lo(bound));
    int x = std::min(int(t.x * resolution.x), resolution.x - 1);
    int y = std::min(int(t.y * resolution.y), resolution.y - 1);
    int z = std::min(int(t.z * resolution.z), resolution.z - 1);
    // use trilinear interpolation
    Spectrum ret = (1 - t.x) * (1 - t.y) * (1 - t.z) * sigma_a[x + y * resolution.x + z * resolution.x * resolution.y];
    ret += (1 - t.x) * (1 - t.y) * t.z * sigma_a[x + y * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += (1 - t.x) * t.y * (1 - t.z) * sigma_a[x + (y + 1) * resolution.x + z * resolution.x * resolution.y];
    ret += (1 - t.x) * t.y * t.z * sigma_a[x + (y + 1) * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += t.x * (1 - t.y) * (1 - t.z) * sigma_a[(x + 1) + y * resolution.x + z * resolution.x * resolution.y];
    ret += t.x * (1 - t.y) * t.z * sigma_a[(x + 1) + y * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += t.x * t.y * (1 - t.z) * sigma_a[(x + 1) + (y + 1) * resolution.x + z * resolution.x * resolution.y];
    ret += t.x * t.y * t.z * sigma_a[(x + 1) + (y + 1) * resolution.x + (z + 1) * resolution.x * resolution.y];
    return ret;
  }
  Spectrum getScatter(const Vec3& p) const {
    // calc the scattering coefficient at point p
    // use trilinear interpolation
    Vec3 t = (p - lo(bound)) / (hi(bound) - lo(bound));
    int x = std::min(int(t.x * resolution.x), resolution.x - 1);
    int y = std::min(int(t.y * resolution.y), resolution.y - 1);
    int z = std::min(int(t.z * resolution.z), resolution.z - 1);
    // use trilinear interpolation
    Spectrum ret = (1 - t.x) * (1 - t.y) * (1 - t.z) * sigma_s[x + y * resolution.x + z * resolution.x * resolution.y];
    ret += (1 - t.x) * (1 - t.y) * t.z * sigma_s[x + y * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += (1 - t.x) * t.y * (1 - t.z) * sigma_s[x + (y + 1) * resolution.x + z * resolution.x * resolution.y];
    ret += (1 - t.x) * t.y * t.z * sigma_s[x + (y + 1) * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += t.x * (1 - t.y) * (1 - t.z) * sigma_s[(x + 1) + y * resolution.x + z * resolution.x * resolution.y];
    ret += t.x * (1 - t.y) * t.z * sigma_s[(x + 1) + y * resolution.x + (z + 1) * resolution.x * resolution.y];
    ret += t.x * t.y * (1 - t.z) * sigma_s[(x + 1) + (y + 1) * resolution.x + z * resolution.x * resolution.y];
    ret += t.x * t.y * t.z * sigma_s[(x + 1) + (y + 1) * resolution.x + (z + 1) * resolution.x * resolution.y];
    return ret;
  }
  virtual ~Medium() = default;
};
struct MediumInterface {
  int internal_id = -1;
  int external_id = -1;
  MediumInterface(int internal, int external) : internal_id(internal), external_id(external) {}
};
}
#endif //RENDERCRAFT_MEDIUM_H
