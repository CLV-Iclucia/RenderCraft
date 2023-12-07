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
// resource management: shapes are managed by Primitives
// others are directly managed by Scene using unique_ptr and STL
// Textures are somewhat difficult to handle, for now we directly instantiate Texture<Vec3> and Texture<Real>
// we can do this because almost all the objects live as long as Scene
struct Scene : NonCopyable {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Aggregate> pr;
  std::unique_ptr<EnvMap> envMap;
  MemoryManager<AreaLight> areaLights;
  MemoryManager<Material> materials;
  MemoryPool<Transform> transforms;
  MemoryManager<Medium> media;
  std::vector<Mesh> meshes;
  Real worldBoundRadius = 0;
  std::unique_ptr<Sampler> sampler;
  Light *sampleLight(Real *pdf) const {
    // TODO: implement this!
  }
  Real pdfSampleLight() const {
    // TODO: implement this!
  }
  Real getExternalRefractionRate(MediumInterface interface) {
  }
  Real getInternalRefractionRate(MediumInterface interface) {
  }
};
}
#endif