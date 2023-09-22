#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include <Core/Medium.h>
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
#include <Core/Medium.h>
namespace rdcraft {
class Integrator;
struct Scene {
  std::unique_ptr<Camera> camera;
  Aggregate *pr;
  std::shared_ptr<EnvMap> envMap = nullptr;
  std::shared_ptr<Integrator> integrator;
  std::vector<AreaLight *> areaLights;
  std::vector<Medium> media;
  void render(); ///< compute the radiance of pixel (x, y)
  Light *sampleLight(Real *pdf) const {
    // TODO: implement this!
  }
  Real pdfSampleLight() const {
    // TODO: implement this!
  }
  Real getExternalRefractionRate(MediumInterface interface) {
    return interface.external_id == -1 ? 1.0 : 1.0 / media[interface.external_id].eta;
  }
  Real getInternalRefractionRate(MediumInterface interface) {
    return interface.internal_id == -1 ? 1.0 : 1.0 / media[interface.internal_id].eta;
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

static inline bool testVisibility(const Vec3& a, const Vec3& b, Scene* scene) {
    Ray test_ray(a, normalize(b - a));
    return !scene->pr->intersect(test_ray);
}
}
#endif