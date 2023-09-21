//
// Created by creeper on 23-3-16.
//

#include <Core/Integrator.h>
#include <Core/maths.h>
#include <Core/Record.h>
#include <Core/Primitive.h>
namespace rdcraft {
Spectrum PathTracer::nextEventEst(const SurfaceRecord &bRec) const {
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
  Real t = toLight.norm();
  toLight /= t;
  Ray neeRay(pos, toLight);
  Spectrum L_nee(1.0);
  if (scene->testVisibility(pos, lightPos)) {
    L_nee += light->evalEmission(pn, -toLight) / pdfLight;
    // TODO: Finish NEE with MIS here
  }
}

Spectrum PathTracer::L(const Ray& ray, Scene* scene) const {
  Spectrum throughput(1.0);
  Spectrum L;
  Vec3 wo = ray.dir;
  for (int bounce = 0;; bounce++) {
    if (get_random() > opt.PRR) break;
    SurfaceRecord rec;
    if (!(scene->pr->intersect(ray, &rec)) || rec.pr->isLight()) {
      if (bounce) {
        // TODO: Implement MIS here
      } else {
        if (scene->envMap)
          L += scene->envMap->evalEmission(wo);
        else {
          L += rec.pr->getLight()->evalEmission();
        }
      }
    }
    Material *mat = rec.pr->getMaterial();
    Mat3 TBN = constructFrame(rec.normal);
    wo = -ray.dir;
    wo = TBN * wo;
    const Vec2 &uv = rec.uv;
    const Vec3 &P = rec.pos;
    Real pdf;
    Vec3 wi = mat->sample(wo, &pdf, uv);
    L += throughput * nextEventEst(rec);
    if (!(scene->pr->intersect(Ray(P, wi)))) {
      Spectrum dir_rad = scene->envMap->evalEmission(wi);
      Real cosThetaI = std::abs(wi.z);
      L += throughput * mat->BxDF(wi, wo, uv) * cosThetaI * dir_rad / pdf;
    }

    wi = mat->sample(wo, &pdf, uv);
    wi = inverse(TBN) * wi;
    const Real cosThetaI = std::abs(wi[2]);
    throughput *= mat->BxDF(wi, wo, uv) / pdf * cosThetaI / opt.PRR;
    wo = wi;
  }
  return L;
}

// TODO: Implement volumetric path tracing
Spectrum VolumePathTracer::render() const {

}
}