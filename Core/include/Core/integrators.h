//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_INTEGRATOR_H
#define RENDERCRAFT_INTEGRATOR_H

#include <Core/ray.h>
#include <Core/scene.h>
#include <Core/spectrums.h>
#include <Core/core.h>
#include <string>

namespace rdcraft {
// this is left for BDPT
struct PathVertex {
};

class Integrator : NonCopyable {
  public:
    virtual void render(const Scene* scene) const = 0;
    virtual ~Integrator() = default;
};

class PathTracer final : public Integrator {
  public:
    struct RenderOptions {
      int maxDepth = -1;
      int rrDepth = 5;
      std::string savingPath = "./output.exr";
      int threadNum = 1;
      bool enableDisplayProcess = true;
      ///< whether to display the rendering process
      bool enableLogOutputs = false; ///< whether to output the rendering log
    } opt;
    Spectrum L(const Ray&, const Scene* scene) const;
    void render(const Scene* scene) const override;
};

class VolumePathTracer final : public Integrator {
  struct RenderOptions {
    const uint spp = 256;
    std::string savingPath = "./output.exr";
    bool enableDisplayProcess = true;
    ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
    // many more to be added
  };

  public:
    void render(const Scene* scene) const override;
};

// I wish one day I could achieve this
// that day will mark the
class BidirPathTracer : public Integrator {
  public:
    void render(const Scene* scene) const override;

  private:
    struct RenderOptions {
      const uint spp = 256;
      std::string savingPath;
      bool enableDisplayProcess = true;
      ///< whether to display the rendering process
      bool enableLogOutputs = false; ///< whether to output the rendering log
    };
};
}
#endif //RENDERCRAFT_INTEGRATOR_H