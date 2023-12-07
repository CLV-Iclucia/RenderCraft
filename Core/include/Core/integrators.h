//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include <Core/ray.h>
#include <Core/Scene.h>
#include <Core/spectrums.h>
#include <Core/core.h>
#include <string>

namespace rdcraft {
// this is left for BDPT
struct PathVertex {

};

class Integrator {
 public:
  virtual void render(Scene* scene) const = 0;
  virtual ~Integrator() = default;
};

class PathTracer final : public Integrator {
 private:
  Spectrum nextEventEst(const SurfaceRecord &bRec, Scene* scene) const;
 public:
  struct RenderOptions {
    Real PRR = 0.95;
    int maxDepth = -1;
    int rrDepth = 5;
    std::string savingPath = "./output.ppm";
    int threadNum = 1;
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
  } opt;
  Spectrum L(const Ray&, Scene* scene) const;
  void render(Scene* scene) const override;
};

class VolumePathTracer : public Integrator {
 private:
  struct RenderOptions {
    const uint spp = 256;
    const Real PRR = 0.95;
    std::string savingPath;
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
    // many more to be added
  };
 public:
  void render(Scene* scene) const override;
};

class BidirPathTracer : public Integrator {
 private:
  struct RenderOptions {
    const uint spp = 256;
    const Real PRR = 0.95;
    std::string savingPath;
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
  };
 public:
  void render(Scene* scene) const override;
};

}
#endif //RENDERCRAFT_INTEGRATOR_H
