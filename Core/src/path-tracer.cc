//
// Created by creeper on 23-3-16.
//

#include <Core/integrators.h>
#include <Core/interaction.h>
#include <Core/scene.h>
#include <Core/image-io.h>
#include <Core/parallel.h>
#include <fstream>

namespace rdcraft {
static bool testVisibility(const Vec3& a, const Vec3& b, const Scene* scene) {
  Ray test_ray(a, normalize(b - a));
  std::optional<SurfaceInteraction> si;
  scene->pr->intersect(test_ray, si);
  return si ? distance(si->pos, b) < epsilon<Real>() : false;
}

static Spectrum
nextEventEst(const SurfaceInteraction& bRec, const Scene* scene) {
  Vec3 pos = bRec.pos;
  Vec3 normal = bRec.normal;
  auto [light, pLight] = scene->sampleLight();
  auto [pn, pdfLightPoint] = light->sample(*scene->sampler);
  Vec3 lightPos = pn.p;
  pdfLightPoint *= pLight;
  Vec3 toLight = lightPos - pos;
  Real t = length(toLight);
  toLight /= t;
  Ray neeRay(pos, toLight);
  Spectrum L_nee(1.0);
  if (testVisibility(pos, lightPos, scene)) {
    L_nee += light->evalEmission(pn, -toLight) / pdfLightPoint;
    // TODO: Finish NEE with MIS here
  }
  return L_nee;
}

Spectrum PathTracer::L(const Ray& ray, const Scene* scene) const {
  Spectrum throughput(1.0);
  Spectrum L;
  Vec3 wo = ray.dir;
  Vec3 normalCache{};
  Real pdfBxdfCache{}; // used for MIS
  for (int bounce = 0;; bounce++) {
    if (bounce >= opt.rrDepth) {
      // TODO: rr
    }
    std::optional<SurfaceInteraction> interaction;
    scene->pr->intersect(ray, interaction);
    if (!interaction) {
      // env map
      if (bounce) {
        // TODO: Implement MIS for env map here
      } else {
        if (scene->envMap)
          L += scene->envMap->evalEmission({}, wo);
        break;
      }
    } else if (interaction->pr->isLight()) {
      auto light = interaction->pr->getLight();
      Real pLight = scene->probSampleLight(light);
      SurfacePatch lightPatch{interaction->pos, interaction->normal};
      Real pdfLight = pLight * light->pdfSample(lightPatch); // area measure
      Real G = computeGeometry({ray.orig, normalCache}, lightPatch);
      if (G == 0.0) break;
      Real pdfBxdf = G * pdfBxdfCache;
      // pdfBxdfCache: solid angle measure, pdfBxdf: area measure
      Real weight = pdfLight * pdfLight / (
                      pdfBxdf * pdfBxdf + pdfLight * pdfLight);
      L += weight * throughput * light->evalEmission(lightPatch, -ray.dir);
      break;
    } else if (interaction->pr->isMediumInterface()) {
      ERROR(
          "Encountered medium interface in the scene, which cannot be handled by naive path tracer.\n"
          "Consider using volumetric path tracer instead.");
    }
    Material* mat = interaction->pr->getMaterial();
    ShadingInfo si{*interaction, -ray.dir};
    L += throughput * nextEventEst(interaction.value(), scene);
    auto optRec{mat->sample(si, *scene->sampler)};
    if (!optRec) break;
    const auto& wi = optRec->wi;
    Real pdfBxdf = optRec->pdf;
    pdfBxdfCache = pdfBxdf;
    normalCache = interaction->normal;
    const Real cosThetaI = std::abs(wi.z);
    throughput *= mat->computeScatter(si) / pdfBxdf * cosThetaI;
    wo = wi;
  }
  return L;
}

void PathTracer::render(const Scene* scene) const {
  ImageIO image(scene->camera->nx, scene->camera->ny);
  int nx{static_cast<int>(scene->camera->nx)};
  tbb::parallel_for(0, nx, [this, scene, &image](int i) {
    for (int j = 0; j < scene->camera->ny; j++) {
      Vec3 radiance{};
      // we use non-splatting style since it is easier to code
      for (int k = 0; k < scene->camera->spp; k++) {
        Ray ray{scene->camera->generateRaySample(i, j)};
        radiance += L(ray, scene);
      }
      radiance /= static_cast<Real>(scene->camera->spp);
      image.write(i, j, radiance);
    }
  });
  printf("Finish rendering\n");
  image.exportEXR(opt.savingPath.c_str());
}
}