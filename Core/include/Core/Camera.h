//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_CAMERA_H
#define RENDERCRAFT_CAMERA_H

#include <Core/Ray.h>
#include <Core/core.h>
namespace rdcraft {
struct Camera {
  uint nx, ny;
  Vec3 pos;
  Vec3 dir;
  void castRay(const Vec3 &ray_dir, Ray *ray) const {
    ray->orig = pos;
    ray->dir = glm::normalize(ray_dir);
  }
};
}
#endif //RENDERCRAFT_CAMERA_H
