//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_CAMERA_H
#define RENDERCRAFT_CAMERA_H

#include <Core/transform.h>
#include <Core/filters.h>
#include <Core/ray.h>
#include <Core/core.h>
#include <memory>

namespace rdcraft {
struct Camera {
  uint nx = 768, ny = 576;
  Transform* cameraToWorld = nullptr;
  Transform* worldToCamera = nullptr;
  Real scrWidth = 4, scrHeight = 4;
  Real focalDistance = 1000.0;
  Real nearPlane = 1e-2;
  Real farPlane = 1e4;
  uint spp = 256;
  int medium_id = -1;
  std::unique_ptr<Filter> filter = nullptr;
  // generate a ray at random on pixel(x, y) in camera space
  // the orig is always at (0, 0, 0) in camera space
  Ray sampleRay(Sampler& sampler, int x, int y) const {
    Vec2 offset = filter->sample(sampler);
    Vec3 orig{};
    Real spacing_x = scrWidth / nx;
    Real spacing_y = scrHeight / ny;
    Vec3 scrPos{(x + 0.5 + offset.x) * spacing_x - 0.5 * scrWidth,
             (y + 0.5 + offset.y) * spacing_y - 0.5 * scrHeight, -nearPlane};
    if (!cameraToWorld)
      return {orig, normalize(scrPos)};
    return cameraToWorld->transform(Ray(orig, scrPos));
  }
};
}
#endif //RENDERCRAFT_CAMERA_H