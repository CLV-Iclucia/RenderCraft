#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include <vector>
#include <Core/Record.h>
#include <Core/Ray.h>
#include <algorithm>
#include <memory>
#include <Core/BVH.h>
#include <Core/Light.h>
#include <Core/EnvMap.h>
#include <Core/Filter.h>
#include <Core/Camera.h>
#include <Core/Primitive.h>
#include <Core/maths.h>
namespace rdcraft {
class Integrator;
struct Scene {
  std::shared_ptr<Filter> filter;
  Camera camera;
  Aggregate *pr;
  BVH *BVHTree = nullptr;
  std::shared_ptr<EnvMap> envMap = nullptr;
  std::shared_ptr<Integrator> integrator;
  std::vector<AreaLight *> areaLights;
  void render(); ///< compute the radiance of pixel (x, y)
  Light *sampleLight(Real *pdf) const {
    // TODO: implement this!
  }
  Real pdfSampleLight() const {

  }
};

struct Visibility {
  Vec3 a;
  Vec3 b;
  bool test(Scene* scene) {
    Ray test_ray(a, normalize(b - a));
    return !scene->pr->intersect(test_ray);
  }
};
}
#endif