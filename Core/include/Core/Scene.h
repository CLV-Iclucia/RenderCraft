#ifndef RENDERCRAFT_SCENE_H
#define RENDERCRAFT_SCENE_H
#include "Core/Transform.h"
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
#include <Core/maths.h>
#include <Core/Medium.h>
#include <Core/Primitive.h>
#include <Core/Texture.h>
#include <Core/Mesh.h>
namespace rdcraft {
class Integrator;
// resource management: shapes are managed by Primitives
// others are directly managed by Scene using unique_ptr and STL
// Textures are somewhat difficult to handle, for now we directly instantiate Texture<Vec3> and Texture<Real>
// we can do this because almost all the objects live as long as Scene
struct Scene {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Aggregate> pr;
  std::unique_ptr<EnvMap> envMap;
  std::vector<AreaLight> areaLights;
  std::vector<std::unique_ptr<Primitive>> primitives; // used for resource management
  std::vector<std::unique_ptr<Material>> materials;
  std::vector<Transform> transforms;
  std::vector<Medium> media;
  std::vector<Mesh> meshes;
  void render(); ///< compute the radiance of pixel (x, y)
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