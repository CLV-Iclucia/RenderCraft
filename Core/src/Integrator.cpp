//
// Created by creeper on 23-3-16.
//

#include <Core/Integrator.h>
#include <Core/maths.h>
#include <Core/Record.h>
#include <Core/Primitive.h>
#include <Core/Scene.h>
#include <iostream>
#include <fstream>
namespace rdcraft {
Spectrum PathTracer::nextEventEst(const SurfaceRecord &bRec, Scene* scene) const {
  int neeDep = 0;
  Real pdfLight;
  Vec3 pos = bRec.pos;
  Vec3 normal = bRec.normal;
  Light *light = scene->sampleLight(&pdfLight);
  Real pdfLightPoint;
  Patch pn = light->sample(pos, &pdfLightPoint);
  Vec3 lightPos = pn.p;
  pdfLight *= pdfLightPoint;
  Vec3 toLight = lightPos - pos;
  Real t = length(toLight);
  toLight /= t;
  Ray neeRay(pos, toLight);
  Spectrum L_nee(1.0);
  if (testVisibility(pos, lightPos, scene)) {
    L_nee += light->evalEmission(pn, -toLight) / pdfLight;
    // TODO: Finish NEE with MIS here
  }
}

Spectrum PathTracer::L(const Ray& ray, Scene* scene) const {
  Spectrum throughput(1.0);
  Spectrum L;
  Vec3 wo = ray.dir;
  for (int bounce = 0;; bounce++) {
    if (randomReal() > opt.PRR) break;
    SurfaceRecord rec;
    if (!(scene->pr->intersect(ray, &rec)) || rec.pr->isLight()) {
      if (bounce) {
        // TODO: Implement MIS here
      } else {
        if (scene->envMap)
          L += scene->envMap->evalEmission(wo);
        else {
          // L += rec.pr->getLight()->evalEmission();
          // TODO: implement MIS for direct light here
          break;
        }
      }
    }
    Material *mat = rec.pr->getMaterial();
    Mat3 TBN = constructFrame(rec.normal);
    wo = -ray.dir;
    wo = TBN * wo;
    Real pdf;
    Vec3 wi = mat->sample(rec, wo, &pdf);
    L += throughput * nextEventEst(rec, scene);
    if (!(scene->pr->intersect(Ray(rec.pos, wi)))) {
      Spectrum dir_rad = scene->envMap->evalEmission(wi);
      Real cosThetaI = std::abs(wi.z);
      L += throughput * mat->BxDF(rec, wo, wi) * cosThetaI * dir_rad / pdf;
    }
    wi = mat->sample(rec, wo, &pdf);
    wi = inverse(TBN) * wi;
    const Real cosThetaI = std::abs(wi[2]);
    throughput *= mat->BxDF(rec, wo, wi) / pdf * cosThetaI / opt.PRR;
    wo = wi;
  }
  return L;
}

void PathTracer::render(Scene* scene) const {
  std::fstream result(opt.savingPath, std::ios_base::out);
    result << "P3" << std::endl << scene->camera->nx << " " << scene->camera->ny << std::endl << 255 << std::endl;
    for (int j = static_cast<int>(scene->camera->ny) - 1; j >= 0; j--) {
      for (int i = 0; i < scene->camera->nx; i++) {
        Vec3 radiance;
        Ray ray;
        for (int k = 0; k < opt.spp; k++) {
          Vec2 offset = scene->camera->filter->sample();
          const Real rx = (i + offset[0]) * opt.scrWid / scene->camera->nx - opt.scrWid * 0.5;
          const Real ry = (j + offset[1]) * opt.scrHeight / scene->camera->ny - opt.scrHeight * 0.5;
          scene->camera->castRay(Vec3(rx, ry, -opt.scrZ), &ray);
          radiance += L(ray, scene) / scene->camera->filter->pdfSample(offset.x, offset.y);
        }
        radiance /= static_cast<Real>(opt.spp);
        if (radiance[0] >= 1.0) radiance[0] = 1.0;
        if (radiance[1] >= 1.0) radiance[1] = 1.0;
        if (radiance[2] >= 1.0) radiance[2] = 1.0;
        radiance[0] = std::sqrt(radiance[0]);
        radiance[1] = std::sqrt(radiance[1]);
        radiance[2] = std::sqrt(radiance[2]);
        result << int(radiance[0] * 255.99) << " " << int(radiance[1] * 255.99) << " " << int(radiance[2] * 255.99)
              << std::endl;
      }
      if (opt.enableDisplayProcess) {
        fflush(stdout);
        printf("%.2lf %% rendered\n", static_cast<Real>(scene->camera->ny - j) * 100.0 / scene->camera->ny);
      }
    }
    fflush(stdout);
    printf("Finish rendering\n");
  }
}