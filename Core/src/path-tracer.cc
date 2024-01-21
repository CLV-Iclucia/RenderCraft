//
// Created by creeper on 23-3-16.
//

#include <Core/integrators.h>
#include <Core/interaction.h>
#include <Core/scene.h>
#include <Core/image-io.h>
#include <Core/parallel.h>
#include <Core/utils.h>
#include <fstream>

namespace rdcraft {
static bool testVisibility(const Vec3& a, const Vec3& b, const Scene* scene) {
  Ray test_ray(a, normalize(b - a));
  std::optional<SurfaceInteraction> si;
  scene->pr->intersect(test_ray, si);
  return si ? scene->spatiallyApproximate(si->pos, b) : false;
}

static Spectrum
nextEventEst(const SurfaceInteraction& bRec, const ShadingInfo& si,
             const Scene* scene) {
  Vec3 pos = bRec.pos;
  Vec3 normal = bRec.normal;
  auto [light, pLight] = scene->sampleLight();
  auto [pn, pdfLightPoint] = light->sample(*scene->sampler);
  Vec3 lightPos = pn.p;
  if (!testVisibility(pos, lightPos, scene)) return {};
  pdfLightPoint *= pLight;
  Real pdfBxdf = bRec.pr->getMaterial()->pdfSample(si, si.wi);
  Real G = computeGeometry({pos, normal}, pn);
  if (G == 0.0) return {};
  pdfBxdf *= G;
  Real weight = pdfLightPoint * pdfLightPoint / (
                  pdfBxdf * pdfBxdf + pdfLightPoint * pdfLightPoint);
  Vec3 toLight = lightPos - pos;
  Real t = length(toLight);
  toLight /= t;
  Spectrum L_nee(1.0);
  L_nee += weight * scene->envMap->evalEmission({}, -toLight) *
      bRec.pr->getMaterial()->computeScatter(si) / pdfLightPoint;
  return L_nee;
}

Spectrum PathTracer::L(const Ray& ray, const Scene* scene) const {
  Spectrum throughput(1.0);
  Spectrum L;
  Vec3 wo = ray.dir;
  Vec3 normalCache{};
  Real pdfBxdfCache{}; // used for MIS, in solid angle measure
  Real etaScale = 1.0;
  for (int bounce = 0;; bounce++) {
    if (bounce >= opt.rrDepth) {
      Real rrProb = std::min(Max<Real, 3>(throughput) / etaScale, 0.95);
      if (scene->sampler->sample() > rrProb) break;
      throughput /= rrProb;
    }
    std::optional<SurfaceInteraction> interaction;
    scene->pr->intersect(ray, interaction);
    if (!interaction && !scene->envMap) break;
    bool handleLight = !interaction || interaction->pr->isLight();
    Light* light = interaction
                     ? interaction->pr->getLight()
                     : scene->envMap.get();
    if (handleLight) {
      if (bounce) {
        Real pLight = scene->probSampleLight(light);
        SurfacePatch lightPatch;
        if (interaction)
          // has interaction means we hit a light, else we hit envmap
          lightPatch = {interaction->pos, interaction->normal};
        else
          lightPatch.n = -ray.dir;
        Real pdfLight = pLight * light->pdfSample(lightPatch);
        Real G = computeGeometry({ray.orig, normalCache}, lightPatch);
        if (G == 0.0) break;
        Real pdfBxdf = G * pdfBxdfCache;
        Real weight = pdfLight * pdfLight / (
                        pdfBxdf * pdfBxdf + pdfLight * pdfLight);
        L += weight * throughput * scene->envMap->evalEmission(
            lightPatch, -ray.dir);
      } else {
        if (scene->envMap)
          L += scene->envMap->evalEmission({}, wo);
      }
      break;
    }
    if (interaction->pr->isMediumInterface()) {
      ERROR(
          "Encountered medium interface in the scene, which cannot be handled by naive path tracer.\n"
          " Consider using volumetric path tracer instead.");
    }
    Material* mat = interaction->pr->getMaterial();
    if (auto optTrans = mat->getTransmissive())
      etaScale *= *optTrans;
    ShadingInfo si{*interaction, -ray.dir};
    L += throughput * nextEventEst(interaction.value(), si, scene);
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
  int ny{static_cast<int>(scene->camera->ny)};
  // use x as the inner loop, because that is how the image is stored
  tbb::parallel_for(0, ny, [&](int j) {
    for (int i = 0; i < scene->camera->nx; i++) {
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