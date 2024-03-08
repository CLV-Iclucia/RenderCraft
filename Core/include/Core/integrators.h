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
// struct PathVertex {
// };

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

class VolumetricPathTracer final : public Integrator {
  public:
    explicit VolumetricPathTracer(const std::string& savingPath) {
      opt.savingPath = savingPath;
    }
    struct RenderOptions {
      const int maxDepth = -1;
      const int rrDepth = 5;
      const uint maxNullCollisions = 1000;
      // simply to control the rendering time
      std::string savingPath = "./output.exr";
      bool enableDisplayProcess = true;
      ///< whether to display the rendering process
      bool enableLogOutputs = false; ///< whether to output the rendering log
      // many more to be added
    } opt;
    void render(const Scene* scene) const override;
    Spectrum L(Ray&, const Scene* scene) const;

  private:
    Spectrum nee(const Ray& ray, int depth, int cur_medium_id,
                 const Scene* scene) const;
    Spectrum neeBxdf(const Ray& ray, int depth,
                     const SurfaceInteraction& si,
                     int cur_medium_id, const Scene* scene) const;
};

// I wish one day I could achieve this
class BidirPathTracer final : public Integrator {
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