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
  Real scrWidth = 4, scrHeight = 4, scrZ = -1;
  Real focalDistance = 1000.0;
  Real nearPlane = 1e-2;
  Real farPlane = 1e4;
  uint spp = 256;
  int medium_id = -1;
  std::unique_ptr<Filter> filter = nullptr;
  // generate a ray at random on pixel(x, y) in camera space
  // the orig is always at (0, 0, 0) in camera space
  Ray generateRaySample(int x, int y) const {
    Vec2 offset = filter->sample();
    Vec3 orig{};
    Real spacing = scrWidth / nx;
    Vec3 dir{(x + offset.x - nx / 2) * spacing,
             (y + offset.y - ny / 2) * spacing, nearPlane};
    return {cameraToWorld->apply(orig), normalize(cameraToWorld->apply(dir))};
  }
};
}
#endif //RENDERCRAFT_CAMERA_H