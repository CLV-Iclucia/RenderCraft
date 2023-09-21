//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include <Core/Camera.h>
#include <Core/Filter.h>
#include <Core/Ray.h>
#include <Core/RenderOptions.h>
#include <Core/Scene.h>
#include <Core/Spectrums.h>
#include <Core/core.h>
#include <memory>
#include <string>

namespace rdcraft {
class Integrator {
 protected:
  std::shared_ptr<Scene> scene = nullptr;
 public:
  virtual Spectrum render() const = 0;
};

class PathTracer final : public Integrator {
 private:
  struct RenderOptions {
    const uint spp = 256;
    const Real PRR = 0.95;
    const Real scrWid = 4, scrHeight = 4, scrZ = -1;
    std::string savingPath = "./output.ppm";
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
  };
  Spectrum nextEventEst(const SurfaceRecord &bRec) const;
 public:
  Spectrum L(const Ray&, Scene* scene) const;
  Spectrum render() const override;
};

class VolumePathTracer : public Integrator {
 private:
  struct RenderOptions {
    const uint spp = 256;
    const Real PRR = 0.95;
    const Real scrWid = 4, scrHeight = 4, scrZ = -1;
    std::string savingPath = "./output.ppm";
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
    // many more to be added
  };
 public:
  Spectrum render() const override;
};
}
#endif //RENDERCRAFT_INTEGRATOR_H
