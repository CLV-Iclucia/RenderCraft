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
// Textures are somewhat difficult to handle, however if there is no recursive
// texture referencing then we can use smart ptr to manage them
// and recursive texture is usually not a good idea
// we can do all these because almost all the objects live as long as Scene
struct Scene : NonCopyable {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Aggregate> pr;
  std::unique_ptr<EnvMap> envMap;
  PolymorphicVector<Light> lights;
  PolymorphicVector<Material> materials;
  PolymorphicVector<Medium> media;
  MemoryPool<Transform> transforms;
  MemoryPool<Mesh, 8> meshes;
  Real worldBoundRadius = 1e8;
  mutable std::unique_ptr<Sampler> sampler;
  DiscreteDistribution lightDistribution;
  void buildLightDistribution() {
    std::vector<Real> weights;
    int i = 0;
    Real totalPower = 0.0;
    for (const auto& light : lights.objects()) {
      Real pw = light->power();
      totalPower += pw;
      weights.push_back(pw);
      lightIndices[light.get()] = i++;
    }
    if (envMap) {
      Real pw = envMap->power();
      weights.push_back(envMap->power());
      totalPower += pw;
      lightIndices[envMap.get()] = i++;
    }
    std::ranges::transform(weights, weights.begin(),
                           [totalPower](Real w) { return w / totalPower; });
    lightDistribution.buildFromWeights(weights);
  }
  template <typename T>
  T epsilon() const {
    return worldBoundRadius * std::numeric_limits<T>::epsilon();
  }
  bool spatiallyApproximate(const Vec3& a, const Vec3& b) const {
    return distance(a, b) < epsilon<Real>();
  }
  LightSampleRecord sampleLight() const {
    auto [idx, p] = lightDistribution.sample(*sampler);
    if (idx == lights.size())
      return {nullptr, 1.0};
    return {lights(idx), p};
  }
  Real probSampleLight(Light* light) const {
    int idx = lightIndices.at(light);
    return lightDistribution.prob(idx);
  }

  private:
    std::unordered_map<Light*, int> lightIndices;
};
}
#endif