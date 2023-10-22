//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_CAMERA_H
#define RENDERCRAFT_CAMERA_H

#include "Core/Transform.h"
#include <Core/Filter.h>
#include <Core/Ray.h>
#include <Core/core.h>
#include <memory> 
namespace rdcraft {
struct Camera {
  uint nx = 0, ny = 0;
  Transform* transform;
  Real scrWidth = 4, scrHeight = 4, scrZ = -1;
  Real focalDistance = 1000.0;
  Real nearPlane = 10.0;
  Real farPlane = 2800.0;
  uint spp = 256;
  std::unique_ptr<Filter> filter = nullptr;
  void castRay(const Vec3 &ray_dir, Ray *ray) const {
    ray->orig = Vec3(0.0);
    ray->dir = glm::normalize(ray_dir);
  }
};
}
#endif //RENDERCRAFT_CAMERA_H
