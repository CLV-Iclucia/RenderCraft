#include <Core/integrators.h>
#include <Core/utils.h>
#include <Core/image-io.h>

namespace rdcraft {
static Real computeGeometry(const SurfacePatch& patch, const Vec3& pos) {
  Vec3 dif = pos - patch.p;
  Real dist = length(dif);
  dif /= dist;
  assert(dist > 0.0);
  return std::max(0.0, dot(dif, patch.n)) / (dist * dist);
}

static void updateMedium(const SurfaceInteraction& si, const Ray& ray,
                         int& cur_medium_id) {
  bool to_external = dot(ray.dir, si.normal) >= 0;
  if (si.pr->isMediumInterface()) {
    int interior_medium_id = si.pr->internalMediumId();
    int exterior_medium_id = si.pr->externalMediumId();
    if (exterior_medium_id == interior_medium_id) return;
    if (to_external)
      cur_medium_id = exterior_medium_id;
    else
      cur_medium_id = interior_medium_id;
  }
}

// maybe my implementation of Path tracing is not that correct
// but CSE272 gurauntees that my Volumetric Path Tracing is correct
// or at least unbiased (if you do not count in the limitations in rendering options)
Spectrum VolumetricPathTracer::nee(const Ray& ray, int depth, int cur_medium_id,
                                   const Scene* scene) const {
  auto [light, pLight] = scene->sampleLight();
  auto [lightPatch, pdfLightPoint] = light->sample(*scene->sampler);
  Ray neeRay;
  neeRay.orig = ray.orig;
  neeRay.dir = normalize(lightPatch.p - ray.orig);
  const PhaseFunction* phaseFunction = scene->media(cur_medium_id)->
      phaseFunction();
  Spectrum nee_throughput = phaseFunction->eval(-ray.dir, neeRay.dir);
  int neeDep = 0;
  Real pdfLight = pdfLightPoint * pLight;
  Real G = computeGeometry(lightPatch, ray.orig);
  Real pdf_delta = 1, pdf_ratio = 1.0;
  if (G > 0.0)
    do_nothing();
  while (true) {
    if (opt.maxDepth >= 0 && depth + neeDep >= opt.maxDepth)
      return {};
    std::optional<SurfaceInteraction> si;
    scene->pr->intersect(neeRay, si);
    if (!si) return {};
    if (cur_medium_id >= 0) {
      Real t_hit = distance(si->pos, neeRay.orig);
      int channel = floor(scene->sampler->sample() * 3);
      Spectrum sigma_m = scene->media(cur_medium_id)->getMajorant(neeRay);
      int nullCollisions = 0;
      auto T = Spectrum(1), trans_pdf_delta = Spectrum(1), trans_pdf_ratio =
          Spectrum(1);
      while (true) {
        Real u = scene->sampler->sample();
        if (sigma_m[channel] <= 0.0) break;
        Real t = -log(1.0 - u) / sigma_m[channel];
        if (t >= t_hit) {
          if (si->pr->isMediumInterface()) {
            //it is an index-matching surface, we go through the interface
            neeDep++;
            T *= exp<Real, 3>(-sigma_m * t_hit);
            trans_pdf_delta *= exp<Real, 3>(-sigma_m * t_hit);
            trans_pdf_ratio *= exp<Real, 3>(-sigma_m * t_hit);
          }
          break;
        } // in ratio tracking, we just perform null scattering event
        Spectrum sigma_s = scene->media(cur_medium_id)->scatterCoefficient(
            neeRay.orig);
        Spectrum sigma_t = scene->media(cur_medium_id)->
                           absorptionCoefficient(neeRay.orig) + sigma_s;
        neeRay.orig += t * neeRay.dir;
        Spectrum sigma_n = sigma_m - sigma_t;
        T *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<Real, 3>(sigma_m);
        trans_pdf_delta *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<
          Real, 3>(sigma_m);
        trans_pdf_ratio *= exp<Real, 3>(-sigma_m * t) * sigma_m / Max<
          Real, 3>(sigma_m);
        nullCollisions++;
        if (nullCollisions >= opt.maxNullCollisions) break;
        if (Max<Real, 3>(T) <= 0.0) return {};
        t_hit -= t;
      }
      nee_throughput *= (T / avg<Real, 3>(trans_pdf_ratio));
      pdf_ratio *= avg<Real, 3>(trans_pdf_ratio);
      pdf_delta *= avg<Real, 3>(trans_pdf_delta);
    }
    neeRay.orig = si->pos;
    if (!si->pr->isLight() && !si->pr->isMediumInterface())
      return {};
    if (si->pr->isLight() && si->pr->getLight() != light)
      return {};
    if (scene->spatiallyApproximate(neeRay.orig, lightPatch.p)) {
      // we reach the light source, and we calc the contrib
      Real pdf_phase = phaseFunction->pdfSample(-neeRay.dir, ray.dir)
                       * G * pdf_delta;
      Real pdf_nee = pLight * pdf_ratio;
      Spectrum L_nee = nee_throughput * si->pr->getLight()->evalEmission(
                           lightPatch, -neeRay.dir) * G / pdfLight;
      Real w = pdf_nee * pdf_nee / (
                 pdf_nee * pdf_nee + pdf_phase * pdf_phase);
      return L_nee * w;
    }
    neeRay.orig += scene->epsilon<Real>() * neeRay.dir;
    updateMedium(*si, neeRay, cur_medium_id);
    neeDep++;
  }
}
Spectrum VolumetricPathTracer::neeBxdf(const Ray& ray, int depth,
                                       const SurfaceInteraction& si,
                                       int cur_medium_id,
                                       const Scene* scene) const {
  auto [light, pLight] = scene->sampleLight();
  auto [lightPatch, pdfLightPoint] = light->sample(*scene->sampler);
  Ray neeRay;
  neeRay.orig = ray.orig;
  neeRay.dir = normalize(lightPatch.p - ray.orig);
  const Material* mat = si.pr->getMaterial();
  auto shading_info = ShadingInfo{si, -ray.dir};
  shading_info.local_wi = shading_info.TBN * neeRay.dir;
  Spectrum nee_throughput = mat->computeScatter(shading_info);
  int neeDep = 0;
  Real pdfLight = pdfLightPoint * pLight;
  Real G = computeGeometry(lightPatch, ray.orig);
  if (G > 0.0)
    do_nothing();
  Real pdf_delta = 1, pdf_ratio = 1;
  updateMedium(si, neeRay, cur_medium_id);
  while (true) //we do a next event estimation
  {
    if (opt.maxDepth >= 0 && depth + neeDep >= opt.maxDepth)
      return {};
    std::optional<SurfaceInteraction> opt_si;
    scene->pr->intersect(neeRay, opt_si);
    if (opt_si) {
      if (cur_medium_id >= 0) {
        if (G > 0)
          do_nothing();
        Real t_hit = distance(opt_si->pos, neeRay.orig);
        int channel = floor(scene->sampler->sample() * 3);
        Spectrum sigma_m = scene->media(cur_medium_id)->getMajorant(neeRay);
        int nullCollisions = 0;
        auto T = Spectrum(1), trans_pdf_delta = Spectrum(1), trans_pdf_ratio =
            Spectrum(1);
        while (true) {
          Real u = scene->sampler->sample();
          if (sigma_m[channel] <= 0.0) break;
          Real t = -log(1.0 - u) / sigma_m[channel];
          if (G > 0)
            do_nothing();
          if (t >= t_hit) {
            if (opt_si->pr->isMediumInterface()) {
              //it is a index-matching surface, we go through the interface
              depth++;
              T *= exp<Real, 3>(-sigma_m * t_hit);
              trans_pdf_delta *= exp<Real, 3>(-sigma_m * t_hit);
              trans_pdf_ratio *= exp<Real, 3>(-sigma_m * t_hit);
              neeRay.orig = opt_si->pos;
            }
            break;
          } // in ratio tracking, we just perform null scattering event
          Spectrum sigma_s = scene->media(cur_medium_id)->scatterCoefficient(
              neeRay.orig);
          Spectrum sigma_t = scene->media(cur_medium_id)->
                             absorptionCoefficient(neeRay.orig) + sigma_s;
          neeRay.orig += t * neeRay.dir;
          Spectrum sigma_n = sigma_m - sigma_t;
          T *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<Real, 3>(sigma_m);
          trans_pdf_delta *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<
            Real, 3>(sigma_m);
          trans_pdf_ratio *= exp<Real, 3>(-sigma_m * t) * sigma_m / Max<
            Real, 3>(sigma_m);
          nullCollisions++;
          if (nullCollisions >= opt.maxNullCollisions) break;
          if (Max<Real, 3>(T) <= 0.0) return {};
          t_hit -= t;
        }
        nee_throughput *= (T / avg<Real, 3>(trans_pdf_ratio));
        pdf_ratio *= avg<Real, 3>(trans_pdf_ratio);
        pdf_delta *= avg<Real, 3>(trans_pdf_delta);
      }
      neeRay.orig = opt_si->pos;
      if (!opt_si->pr->isLight() && !opt_si->pr->isMediumInterface())
        return {};
      if (opt_si->pr->isLight() && opt_si->pr->getLight() != light)
        return {};
      if (scene->spatiallyApproximate(neeRay.orig, lightPatch.p)) {
        // we reach the light source, and we calc the contrib
        if (G > 0)
          do_nothing();
        Real pdf_bsdf = mat->pdfSample(shading_info, neeRay.dir) * G *
                        pdf_delta;
        Real pdf_nee = pdfLight * pdf_ratio;
        Spectrum L_nee = nee_throughput * light->evalEmission(
                             lightPatch, -neeRay.dir) * G / pdfLight;
        Real w = pdf_nee * pdf_nee / (pdf_nee * pdf_nee + pdf_bsdf * pdf_bsdf);
        return L_nee * w;
      }
      neeRay.orig += scene->epsilon<Real>() * neeRay.dir;
      updateMedium(*opt_si, neeRay, cur_medium_id);
      neeDep++;
    } else return {};
  }
}

// The final volumetric renderer:
// multiple chromatic heterogeneous volumes with multiple scattering
// with MIS between next event estimation and phase function sampling
// with surface lighting
Spectrum VolumetricPathTracer::L(Ray& ray, const Scene* scene) const {
  int cur_medium_id = scene->camera->medium_id;
  Spectrum cur_path_throughput(1.0, 1.0, 1.0);
  int depth = 0;
  Spectrum L(0.0, 0.0, 0.0);
  Real pdf_ratio = 1, pdf_delta = 1; // p_march for nee
  SurfaceInteraction si_nee_cache;
  Real pdf_dir_cache = 0;
  bool never_scatter_or_reflect = true;
  while (true) {
    if (opt.maxDepth >= 0 && depth >= opt.maxDepth)
      break;
    if (opt.rrDepth >= 0 && depth >= opt.rrDepth) {
      //first do a Russian Roulette
      Real rnd = scene->sampler->sample();
      Real rrProb = std::min(0.95, 1.0 - Max<Real, 3>(cur_path_throughput));
      if (rnd > rrProb) break;
      cur_path_throughput /= rrProb;
    }
    std::optional<SurfaceInteraction> si;
    scene->pr->intersect(ray, si);
    bool reach_light = false;
    bool scatter = false;
    bool surface_lighting = false;
    Spectrum sigma_s;
    Real t_hit;
    SurfacePatch patch;
    if (!si) t_hit = infinity<Real>();
    else {
      patch = {si->pos, si->normal};
      t_hit = distance(ray.orig, si->pos);
    }
    if (cur_medium_id >= 0) {
      Spectrum sigma_t;
      int channel;
      channel = floor(scene->sampler->sample() * 3);
      Spectrum sigma_m = scene->media(cur_medium_id)->getMajorant(ray);
      int nullCollisions = 0;
      auto T = Spectrum(1), trans_pdf_delta = Spectrum(1), trans_pdf_ratio =
          Spectrum(1);
      while (true) {
        // perform free-flight sampling
        Real u = scene->sampler->sample();
        if (sigma_m[channel] <= 0.0) break;
        Real t = -log(1.0 - u) / sigma_m[channel];
        if (t >= t_hit) {
          trans_pdf_delta *= exp<Real, 3>(-t_hit * sigma_m);
          trans_pdf_ratio *= exp<Real, 3>(-t_hit * sigma_m);
          T *= exp<Real, 3>(-t_hit * sigma_m);
          ray.orig = si->pos;
          if (si->pr->isMediumInterface()) {
            //it is a index-matching surface, we go through the interface
            depth++;
            updateMedium(*si, ray, cur_medium_id);
            ray.orig += scene->epsilon<Real>() * ray.dir;
            // avoid self-intersection, especially for medium interface
          } else {
            if (si->pr->isLight()) reach_light = true;
            else surface_lighting = true;
          }
          break;
        }
        // we sample to decide whether it is a null collision or scattering event
        ray.orig += t * ray.dir;
        sigma_s = scene->media(cur_medium_id)->scatterCoefficient(ray.orig);
        sigma_t = scene->media(cur_medium_id)->absorptionCoefficient(ray.orig)
                  + sigma_s;
        if (scene->sampler->sample() < sigma_t[channel] / sigma_m[
              channel]) {
          scatter = true;
          T *= exp<Real, 3>(-sigma_m * t) / Max<Real, 3>(sigma_m);
          trans_pdf_delta *= exp<Real, 3>(-sigma_m * t) * sigma_t / Max<
            Real, 3>(sigma_m);
          break;
        }
        Spectrum sigma_n = sigma_m - sigma_t;
        T *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<Real, 3>(sigma_m);
        trans_pdf_delta *= exp<Real, 3>(-sigma_m * t) * sigma_n / Max<Real, 3>(
            sigma_m);
        trans_pdf_ratio *= exp<Real, 3>(-sigma_m * t) * sigma_m / Max<Real, 3>(
            sigma_m);
        if (Max<Real, 3>(T) <= 0.0) break;
        nullCollisions++;
        if (nullCollisions >= opt.maxNullCollisions) break;
        t_hit -= t;
      }
      cur_path_throughput *= (T / avg<Real, 3>(trans_pdf_delta));
      pdf_delta *= avg<Real, 3>(trans_pdf_delta);
      pdf_ratio *= avg<Real, 3>(trans_pdf_ratio);
    } else {
      if (!si) break;
      ray.orig = si->pos;
      if (si->pr->isLight()) reach_light = true;
      else if (!si->pr->isMediumInterface())
        surface_lighting = true;
      else {
        ray.orig += scene->epsilon<Real>() * ray.dir;
        updateMedium(*si, ray, cur_medium_id);
      }
    }
    if (scatter) {
      cur_path_throughput *= sigma_s;
      Spectrum L_nee = nee(ray, depth, cur_medium_id, scene);
      L += cur_path_throughput * L_nee;
      si_nee_cache = *si;
      never_scatter_or_reflect = false;
      PhaseFunction* phaseFunction = scene->media(cur_medium_id)->
          phaseFunction();
      std::optional<PhaseFunctionSampleRecord> pRec;
      phaseFunction->sample(-ray.dir, *scene->sampler, pRec);
      if (pRec) {
        Vec3 dir = pRec->wi;
        Real phase_pdf = pRec->pdf;
        pdf_dir_cache = phase_pdf;
        pdf_ratio = pdf_delta = 1.0;
        cur_path_throughput *= phaseFunction->eval(-ray.dir, dir) / phase_pdf;
        ray.dir = dir;
      } else break;
    }
    if (reach_light) {
      if (never_scatter_or_reflect)
        L += cur_path_throughput * si->pr->getLight()->evalEmission(
            patch, -ray.dir);
      else {
        Real G = computeGeometry(patch, si_nee_cache.pos);
        Real pdf_dir = pdf_delta * G * pdf_dir_cache;
        Real pdf_nee = scene->probSampleLight(si->pr->getLight()) *
                       si->pr->getLight()->pdfSample(patch);
        Real w = pdf_dir * pdf_dir / (pdf_dir * pdf_dir + pdf_nee * pdf_nee);
        L += cur_path_throughput * si->pr->getLight()->evalEmission(
            patch, -ray.dir) * w;
      }
      break;
    }
    if (surface_lighting) {
      never_scatter_or_reflect = false;
      ShadingInfo shading_info{*si, -ray.dir};
      const Material* mat = si->pr->getMaterial();
      Spectrum L_nee = neeBxdf(ray, depth, *si, cur_medium_id, scene);
      L += cur_path_throughput * L_nee;
      si_nee_cache = *si;
      if (std::optional<BxdfSampleRecord> bRec = mat->sample(
          shading_info, *scene->sampler)) {
        Vec3 world_wi = inverse(shading_info.TBN) * bRec->local_wi;
        Real bsdf_pdf = bRec->pdf;
        if (bsdf_pdf == 0.0) break;
        shading_info.local_wi = bRec->local_wi;
        cur_path_throughput *= mat->computeScatter(shading_info) / bsdf_pdf;
        pdf_dir_cache = bsdf_pdf;
        pdf_delta = pdf_ratio = 1.0;
        ray.dir = world_wi;
        updateMedium(*si, ray, cur_medium_id);
      } else break;
    }
    depth++;
  }
  return L;
}

// It has been almost a year since I finished volumetric path tracing in lajolla...
// But everything is past, and I have to start over again.
// I have to admit that to finish this I adapted my implementation in lajolla to my own renderer almost line by line.
void VolumetricPathTracer::render(const Scene* scene) const {
  ImageIO image(scene->camera->nx, scene->camera->ny);
  int ny{static_cast<int>(scene->camera->ny)};
  int nx{static_cast<int>(scene->camera->nx)};
  // use x as the inner loop, because that is how the image is stored
#ifdef NDEBUG
  tbb::parallel_for (0, ny, [&](int j) {
    for (int i = 0; i < nx; i++) {
      Vec3 radiance{};
      // we use non-splatting style since it is easier to code
      for (int k = 0; k < scene->camera->spp; k++) {
        Ray ray{scene->camera->sampleRay(*scene->sampler, i, j)};
        radiance += L(ray, scene);
      }
      radiance /= static_cast<Real>(scene->camera->spp);
      // because the image is flipped, we need to flip the y coordinate
      image.write(i, ny - 1 - j, radiance);
    }
  });
#else
  for (int j = 0; j < ny; j++) {
    for (int i = 0; i < nx; i++) {
      Vec3 radiance{};
      // we use non-splatting style since it is easier to code
      for (int k = 0; k < scene->camera->spp; k++) {
        Ray ray{scene->camera->sampleRay(*scene->sampler, i, j)};
        radiance += L(ray, scene);
      }
      radiance /= static_cast<Real>(scene->camera->spp);
      // because the image is flipped, we need to flip the y coordinate
      image.write(i, ny - 1 - j, radiance);
    }
  }
#endif
  printf("Finish rendering\n");
  image.exportEXR(opt.savingPath.c_str());
}
}