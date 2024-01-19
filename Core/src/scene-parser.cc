//
// Created by creeper on 9/21/23.
//
#include <Core/transform.h>
#include <Core/integrators.h>
#include <Core/material.h>
#include <Core/mesh.h>
#include <Core/primitive.h>
#include <Core/spectrums.h>
#include <Core/sphere.h>
#include <Core/utils.h>
#include <Core/obj-loader.h>
#include <Core/scene-parser.h>
#include <Core/light.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <format>

namespace rdcraft {

static std::map<std::string, Material*> materialRef;
// since AreaLight can apply to different shapes
// we only map the other members, and construct the light when parsing the shape
static std::map<std::string, Light*> lightRef;
static MemoryManager<Light> lights;
static MemoryManager<Material> materials;
static MemoryManager<Primitive> primitives;
static MemoryPool<Transform> transforms;
static MemoryPool<Mesh, 8> meshes;
static std::unique_ptr<EnvMap> envMap{};
static bool hasEnvMap = false;
// sadly, this parser fails to compile correctly on my machine
// I have to use hard-coded scene
#ifndef USE_HARD_CODED_SCENE
// implement a function that splits a string by a given regex
std::vector<std::string> split(const std::string& str, const std::regex& re) {
  std::sregex_token_iterator first{str.begin(), str.end(), re, -1}, last;
  return {first, last};
}

Vec3 parseVec3(const std::string& value) {
  std::vector<std::string> components = split(value, std::regex("(,| )+"));
  if (components.size() == 1) {
    Real v = std::stof(components[0]);
    return Vec3(v, v, v);
  } else if (components.size() == 3) {
    return Vec3(std::stod(components[0]), std::stod(components[1]),
                std::stod(components[2]));
  } else {
    std::cout << "Unknown Vec3 format: " << value << std::endl;
    exit(-1);
  }
}

Mat4 parseMat4(const std::string& value) {
  std::vector<std::string> components = split(value, std::regex("(,| )+"));
  if (components.size() == 16) {
    Mat4 mat;
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++) {
        mat[i][j] = std::stod(components[i * 4 + j]);
      }
    return mat;
  } else {
    std::cout << "Unknown Mat4 format: " << value << std::endl;
    exit(-1);
  }
}

Transform* parseTransform(pugi::xml_node transform) {
  Mat4 mat;
  mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0;
  for (pugi::xml_node node : transform.children()) {
    if (strcmp(node.name(), "translate") == 0) {
      Vec3 trans(node.attribute("x").as_float(), node.attribute("y").as_float(),
                 node.attribute("z").as_float());
      mat = translate(mat, trans);
    } else if (strcmp(node.name(), "scale") == 0) {
      // scale can either be a vec3 or a float
      if (node.attribute("value").empty()) {
        Vec3 sc(node.attribute("x").as_float(), node.attribute("y").as_float(),
                node.attribute("z").as_float());
        mat = scale(mat, sc);
      } else {
        Real sc = node.attribute("value").as_float();
        mat = scale(mat, Vec3(sc, sc, sc));
      }
    } else if (strcmp(node.name(), "rotate") == 0) {
      Vec3 axis(node.attribute("x").as_float(), node.attribute("y").as_float(),
                node.attribute("z").as_float());
      Real angle = node.attribute("angle").as_float();
      mat = rotate(mat, angle, axis);
    } else if (strcmp(node.name(), "lookAt") == 0) {
      Vec3 eye(node.attribute("origin").as_float(),
               node.attribute("y").as_float(), node.attribute("z").as_float());
      Vec3 target(node.attribute("target").as_float(),
                  node.attribute("y").as_float(),
                  node.attribute("z").as_float());
      Vec3 up(node.attribute("up").as_float(), node.attribute("y").as_float(),
              node.attribute("z").as_float());
      mat = glm::lookAt(eye, target, up);
    } else if (strcmp(node.name(), "matrix") == 0) {
      mat = parseMat4(node.attribute("value").value());
    } else
      ERROR("Unknown transform node: " + std::string(node.name()));
  }
  return transforms.construct(mat);
}

Transform* invTransform(const Transform* transform) {
  return transforms.construct<Transform>(transform->inverse());
}

std::unique_ptr<Integrator> parseIntegrator(pugi::xml_node integrator) {
  std::string integrator_type = integrator.attribute("type").value();
  if (integrator_type == "path") {
    auto path_tracer{std::make_unique<PathTracer>()};
    // for all the integer child nodes
    for (pugi::xml_node integer = integrator.child("integer"); integer;
         integer = integer.next_sibling("integer")) {
      std::string name = integer.attribute("name").value();
      if (name == "maxDepth") {
        path_tracer->opt.maxDepth = integer.attribute("value").as_int();
      } else if (name == "rrDepth") {
        path_tracer->opt.rrDepth = integer.attribute("value").as_int();
      } else {
        std::cout << "Unknown integer attribute: " << name << std::endl;
        exit(-1);
      }
    }
    return std::move(path_tracer);
  }
  if (integrator_type == "volpath")
    return std::make_unique<VolumePathTracer>();
  ERROR("Unknown integrator type: " + integrator_type);
}

std::unique_ptr<Filter> parseFilter(pugi::xml_node filter) {
  ASSERT(filter, "Empty filter");
  std::string filter_type = filter.attribute("type").value();
  if (filter_type == "box")
    return std::make_unique<BoxFilter>(0.5);
  if (filter_type == "gaussian")
    return std::make_unique<GaussianFilter>(1.0, 0.5, 0.5);
  ERROR("Unknown filter type: " + filter_type);
}

void parseFilm(const pugi::xml_node film, Camera* camera) {
  CHECK_NEZ(camera);
  for (pugi::xml_node integer = film.child("integer"); integer;
       integer = integer.next_sibling("integer")) {
    if (std::string name = integer.attribute("name").value(); name == "width") {
      camera->nx = integer.attribute("value").as_int();
    } else if (name == "height") {
      camera->ny = integer.attribute("value").as_int();
    } else
      ERROR("Unknown integer attribute: " + name);
  }
  camera->filter = parseFilter(film.child("rchild"));
}

std::unique_ptr<Camera> parseSensor(const pugi::xml_node sensor) {
  if (std::string sensor_type = sensor.attribute("type").value();
    sensor_type == "perspective") {
    auto perspective_camera = std::make_unique<Camera>();
    Real fov;
    std::string fovAxis = "x";
    for (pugi::xml_node float_node = sensor.child("float"); float_node;
         float_node = float_node.next_sibling("float")) {
      if (std::string name = float_node.attribute("name").value(); name == "fov") {
        fov = float_node.attribute("value").as_float();
      } else if (name == "near_clip") {
        perspective_camera->nearPlane =
            float_node.attribute("value").as_float();
      } else if (name == "far_clip") {
        perspective_camera->farPlane = float_node.attribute("value").as_float();
      } else if (name == "focalDistance") {
        perspective_camera->focalDistance =
            float_node.attribute("value").as_float();
      } else
        ERROR("Unknown float attribute: " + name);
    }
    for (pugi::xml_node float_node = sensor.child("float"); float_node;
         float_node = float_node.next_sibling("float")) {
      if (std::string name = float_node.attribute("name").value(); name == "fov") {
        fov = float_node.attribute("value").as_float();
      } else if (name == "near_clip") {
        perspective_camera->nearPlane =
            float_node.attribute("value").as_float();
      } else if (name == "far_clip") {
        perspective_camera->farPlane = float_node.attribute("value").as_float();
      } else if (name == "focalDistance") {
        perspective_camera->focalDistance =
            float_node.attribute("value").as_float();
      } else
        ERROR("Unknown float attribute: " + name);
    }
    if (pugi::xml_node film = sensor.child("film"))
      parseFilm(film, perspective_camera.get());
    pugi::xml_node sampler = sensor.child("ldsampler");
    ASSERT(sampler, "No sampler found for camera");
    perspective_camera->spp = sampler.attribute("sampleCount").as_int();
    if (sensor.child("toWorld")) {
      perspective_camera->cameraToWorld = parseTransform(
          sensor.child("toWorld"));
      perspective_camera->worldToCamera = invTransform(
          perspective_camera->cameraToWorld);
    } else
      ERROR("No transform node found");
    Real horizontalFov = glm::radians(fov);
    if (fovAxis == "x") {
      Real halfWidth = std::tan(horizontalFov / 2.0) * perspective_camera->
                       nearPlane;
      perspective_camera->scrWidth = 2.0 * halfWidth;
      perspective_camera->scrHeight =
          perspective_camera->scrWidth * perspective_camera->ny /
          perspective_camera->nx;
    }
    return std::move(perspective_camera);
  } else
    ERROR(std::string("Unknown sensor") + sensor_type);
}

Spectrum parseSpectrum(const std::string& value) {
  std::vector<std::string> components = split(value, std::regex("(,| )+"));
  std::vector<std::tuple<int, Real>> distrib;
  for (auto& component : components) {
    auto pos = component.find(":");
    CHECK_EQ(pos, std::string::npos);
    distrib.emplace_back(std::stoi(component.substr(0, pos)),
                         std::stod(component.substr(pos + 1)));
  }
  return AdaptiveSpectrum(distrib).toRGB();
}

Spectrum parseRGB(const pugi::xml_node rgb) {
  // take the value attribute
  // can either be one number or three numbers
  if (std::string value = rgb.attribute("value").as_string();
    value.find(',') == std::string::npos) {
    Real v = std::stod(value);
    return Spectrum(v, v, v);
  } else {
    Vec3 v = parseVec3(value);
    return v;
  }
}

Spectrum parseSpectrum(const pugi::xml_node spectrum) {
  std::string spectrum_type = spectrum.attribute("type").value();
  if (spectrum_type == "uniform") {
    return Spectrum(spectrum.attribute("value").as_float());
  }
  if (spectrum_type == "rgb") {
    return Spectrum(Vec3(spectrum.attribute("value").as_float()));
  }
  if (spectrum_type == "wavelengths") {
    return parseSpectrum(spectrum.attribute("value").as_string());
  } else
    ERROR(std::string("Unknown spectrum") + spectrum_type);
}

Material* parseMaterial(const pugi::xml_node bsdf) {
  std::string bsdf_type = bsdf.attribute("type").value();
  Material* material{};
  if (bsdf_type == "diffuse") {
    if (pugi::xml_node spectrum = bsdf.child("spectrum")) {
      material = materials.construct<Lambertian>(parseSpectrum(spectrum));
    } else
      ERROR(std::string("Unknown bsdf") + bsdf_type);
  } else if (bsdf_type == "metal") {
  } else
    ERROR("Unknown bsdf type: " + bsdf_type);
  ASSERT(!materialRef.contains(bsdf.attribute("id").value()),
         std::format("Duplicate material id: {}", bsdf.attribute("id").value()
         ));
  materialRef[bsdf.attribute("id").value()] = material;
  return material;
}

std::unique_ptr<Shape> parseSphere(const pugi::xml_node sphere) {
  Real radius{};
  Transform* obj_to_world = parseTransform(sphere.child("transform"));
  Transform* world_to_obj = invTransform(obj_to_world);
  radius = sphere.attribute("radius").as_float();
  return std::make_unique<Sphere>(radius, world_to_obj, obj_to_world);
}

// mitsuba doc: an emitter "area" inside a "shape" makes the shape an emitter
Light* parseAreaLight(const pugi::xml_node emitter) {
  if (std::string emitter_type = emitter.attribute("type").value();
    emitter_type == "area") {
    pugi::xml_node radiance = emitter.child("spectrum");
    ASSERT(radiance, "No radiance for area light");
    Spectrum lightRadiance = parseSpectrum(radiance);
    return lights.construct<AreaLight>(lightRadiance);
  } else
    ERROR("Emitter" + emitter_type + "shouldn't be inside a shape");
}

std::unique_ptr<EnvMap> parseEnvMap(pugi::xml_node envMap) {
}

// for lights other than AreaLights, use this
// for now only supports envmap
void parseLight(const pugi::xml_node emitter) {
  if (std::string emitter_type = emitter.attribute("type").value();
    emitter_type == "envmap") {
    ASSERT(envMap == nullptr, "The scene has more than one EnvMaps.");
    envMap = std::move(parseEnvMap(emitter));
  } else if (emitter_type == "constant") {
    ASSERT(envMap == nullptr, "The scene has more than one EnvMaps.");
    // find the rgb node with name "radiance"
    pugi::xml_node rgb = emitter.child("rgb");
    ASSERT(rgb, "No spectrum for the radiance of the envmap");
    ASSERT(rgb.attribute("name").as_string() == "radiance",
           "No spectrum for the radiance of the envmap");
  } else
    ERROR("Emitter type" + emitter_type + "not supported yet");
}

// in Mitsuba's scene format it is called shape
// I follow PBRT's naming convention to call it primitive
void parsePrimitive(const pugi::xml_node primitive) {
  if (primitive.attribute("type").empty())
    ERROR("no type for primitive");
  std::string primitive_type = primitive.attribute("type").value();
  Primitive* pr{};
  std::unique_ptr<Shape> shape{};
  if (primitive_type == "sphere")
    shape = parseSphere(primitive);
  else if (primitive_type == "obj") {
    Mesh* mesh = meshes.construct();
    std::string filename;
    for (pugi::xml_node string_node = primitive.child("string"); string_node;
         string_node = string_node.next_sibling("string")) {
      std::string name = string_node.attribute("name").value();
      if (name == "filename") {
        filename = string_node.attribute("value").value();
      } else
        ERROR(std::string("Unknown attribute") + name);
    }
    loadObj(filename, mesh);
    ASSERT(mesh->indices.size() % mesh->triangleCount != 0,
           "Incomplete mesh info");
    for (int i = 0; i < mesh->triangleCount; i++)
      primitives.construct<Triangle>(mesh->indices.data() + 3 * i, mesh);
  }
  Material* material = nullptr;
  Light* light = nullptr;
  bool isMat = false, isLight = false;
  if (primitive.child("ref")) {
    std::string ref = primitive.child("ref").attribute("id").value();
    isMat = materialRef.contains(ref);
    isLight = lightRef.contains(ref);
    ASSERT(isMat ^ isLight, "Unknown ref: " + ref);
    if (isMat)
      material = materialRef[ref];
    else if (isLight)
      light = lightRef[ref];
  }
  if (primitive.child("bsdf")) {
    // if there is no ref child, then use the bsdf child
    // use parseMaterial
    pugi::xml_node bsdf = primitive.child("bsdf");
    ASSERT(!isMat, "two materials for one shape");
    ASSERT(!isLight, "a surface can't be both a material and a light");
    ASSERT(bsdf, "parsing bsdf failed");
    if (bsdf) {
      material = parseMaterial(bsdf);
      materialRef[bsdf.attribute("id").as_string()] = material;
    }
  } else
    ERROR("No bsdf for shape");
  if (primitive.child("emitter")) {
    ASSERT(!isLight, "two emitters for one shape");
    ASSERT(!isMat, "a surface can't be both a material and a light");
    pugi::xml_node emitter = primitive.child("emitter");
    light = parseAreaLight(emitter);
  }
  primitives.construct<GeometryPrimitive>();
}

std::tuple<std::unique_ptr<Scene>, std::unique_ptr<Integrator>>
loadScene(const char* file_path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(file_path);
  // TODO: implement log lib to replace this
  std::cout << std::format("{} parsing result: {}\n", file_path,
                           result.description());
  std::unique_ptr<Scene> scene{};
  std::unique_ptr<Integrator> integrator{};
  pugi::xml_node root = doc.child("scene");
  ASSERT(!root.empty(), "empty file");
  for (pugi::xml_node node : root.children()) {
    if (strcmp(node.name(), "integrator") == 0) {
      integrator = parseIntegrator(node);
    } else if (strcmp(node.name(), "sensor") == 0) {
      scene->camera = parseSensor(node);
    } else if (strcmp(node.name(), "emitter") == 0) {
      parseLight(node);
    } else if (strcmp(node.name(), "bsdf") == 0) {
      parseMaterial(node);
    } else if (strcmp(node.name(), "shape") == 0) {
      parsePrimitive(node);
    } else
      ERROR(std::string("Unknown node:") + node.name());
  }
  ASSERT(integrator, "No integrator found");
  ASSERT(scene->camera, "No camera found");
  scene->materials = std::move(materials);
  scene->transforms = std::move(transforms);
  scene->lights = std::move(lights);
  scene->buildLightDistribution();
  scene->pr = std::make_unique<Aggregate>(std::move(primitives));
  return std::make_tuple(std::move(scene), std::move(integrator));
}
#else

std::tuple<std::unique_ptr<Scene>, std::unique_ptr<Integrator>> hardCodedScene() {

  auto integrator = std::make_unique<PathTracer>();

}

#endif
} // namespace rdcraft