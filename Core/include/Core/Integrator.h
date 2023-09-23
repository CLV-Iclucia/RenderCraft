//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include <Core/Camera.h>
#include <Core/Filter.h>
#include <Core/Ray.h>
#include <Core/Scene.h>
#include <Core/Spectrums.h>
#include <Core/core.h>
#include <memory>
#include <string>

namespace rdcraft {
class Integrator {
 public:
  virtual void render(Scene* scene) const = 0;
};

class PathTracer final : public Integrator {
 private:
  struct RenderOptions {
    uint spp = 256;
    Real PRR = 0.95;
    int maxDepth = -1;
    std::string savingPath = "./output.ppm";
    int threadNum = 1;
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
  } opt;
  Spectrum nextEventEst(const SurfaceRecord &bRec, Scene* scene) const;
 public:
  void setMaxDepth(int maxDepth) {
    opt.maxDepth = maxDepth;
  }
  void setSavingPath(const std::string& path) {
    opt.savingPath = path;
  }
  void setThreadNum(int threadNum) {
    opt.threadNum = threadNum;
  }
  void setSamplesPerPixel(uint spp) {
    opt.spp = spp;
  }
  Spectrum L(const Ray&, Scene* scene) const;
  void render(Scene* scene) const override;
};

class VolumePathTracer : public Integrator {
 private:
  struct RenderOptions {
    const uint spp = 256;
    const Real PRR = 0.95;
    const Real scrWid = 4, scrHeight = 4, scrZ = -1;
    std::string savingPath;
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
    // many more to be added
  };
 public:
  void render(Scene* scene) const override;
};
}
#endif //RENDERCRAFT_INTEGRATOR_H
