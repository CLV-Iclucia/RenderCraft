#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include <Core/transform.h>
#include <Core/medium.h>
#include <Core/light.h>
#include <Core/camera.h>
#include <Core/primitive.h>
#include <Core/mesh.h>
#include <Core/memory.h>
#include <Core/sampler.h>
#include <memory>
#include <vector>

namespace rdcraft {
class Integrator;

struct LightSampleRecord {
  const Light* light{nullptr};
  Real pdf{0.0};
};

// resource management: shapes are managed by Primitives
// others are directly managed by Scene using unique_ptr and STL
// Textures are somewhat difficult to handle, for now we directly instantiate Texture<Vec3> and Texture<Real>
// we can do this because almost all the objects live as long as Scene
struct Scene : NonCopyable {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Aggregate> pr;
  std::unique_ptr<EnvMap> envMap;
  MemoryManager<Light> lights;
  MemoryManager<Material> materials;
  MemoryPool<Transform> transforms;
  MemoryManager<Medium> media;
  MemoryPool<Mesh> meshes;
  Real worldBoundRadius = 1e4;
  mutable std::unique_ptr<Sampler> sampler;
  DiscreteDistribution lightDistribution;
  void buildLightDistribution() {
    std::vector<Real> weights;
    for (const auto& light : lights.objects())
      weights.push_back(light->power());
    weights.push_back(envMap->power());
    lightDistribution.buildFromWeights(weights);
  }
  LightSampleRecord sampleLight() const {
    auto [idx, p] = lightDistribution.sample();
    if (idx == lights.size())
      return {nullptr, 1.0};
    return {lights(idx), p};
  }
  Real probSampleLight(Light* light) const {
    return lightDistribution.prob(light - lights.objects().data()->get());
  }
};
}
#endif