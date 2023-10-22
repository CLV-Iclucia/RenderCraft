//
// Created by creeper on 9/21/23.
//
#include <Core/Transform.h>
#include <Core/Integrator.h>
#include <Core/Material.h>
#include <Core/Mesh.h>
#include <Core/Primitive.h>
#include <Core/Spectrums.h>
#include <Core/Sphere.h>
#include <Core/maths.h>
#include <Core/obj-loader.h>
#include <Core/scene-parser.h>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
namespace rdcraft {

static std::map<std::string, Material*> material_ref;
static std::map<std::string, std::unique_ptr<Light>> light_ref;
static std::vector<AreaLight> lights;
static std::vector<std::unique_ptr<Material>> materials;
static std::vector<std::unique_ptr<Primitive>> primitives;
static std::vector<Transform> transforms;

// implement a function that splits a string by a given regex
std::vector<std::string> split(const std::string &str, const std::regex &re) {
  std::sregex_token_iterator first{str.begin(), str.end(), re, -1}, last;
  return {first, last};
}

Vec3 parseVec3(const std::string &value) {
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

Mat4 parseMat4(const std::string &value) {
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
  Mat4 mat = Mat4(1.0);
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
    } else {
      std::cout << "Unknown transform: " << node.name() << std::endl;
      exit(-1);
    }
  }
  transforms.emplace_back(mat);
  return transforms.data() + transforms.size() - 1;
}

Transform* invTransform(Transform* transform) {
  transforms.emplace_back(transform->inverse());
  return transforms.data() + transforms.size() - 1;
}

std::unique_ptr<Integrator> parseIntegrator(pugi::xml_node integrator) {
  std::string integrator_type = integrator.attribute("type").value();
  if (integrator_type == "path") {
    std::unique_ptr<PathTracer> path_tracer = std::make_unique<PathTracer>();
    // for all the integer child nodes
    for (pugi::xml_node integer = integrator.child("integer"); integer;
         integer = integer.next_sibling("integer")) {
      std::string name = integer.attribute("name").value();
      if (name == "maxDepth") {
        path_tracer->setMaxDepth(integer.attribute("value").as_int());
      } else if (name == "rrDepth") {
        path_tracer->setRRDepth(integer.attribute("value").as_int());
      } else {
        std::cout << "Unknown integer attribute: " << name << std::endl;
        exit(-1);
      }
    }
    return std::make_unique<PathTracer>();
  } else if (integrator_type == "volpath") {
    return std::make_unique<VolumePathTracer>();
  } else {
    std::cout << "Unknown integrator: " << integrator_type << std::endl;
    exit(-1);
  }
}

void parseFilm(pugi::xml_node film, Camera *camera) {
  for (pugi::xml_node integer = film.child("integer"); integer;
       integer = integer.next_sibling("integer")) {
    std::string name = integer.attribute("name").value();
    if (name == "width") {
      camera->nx = integer.attribute("value").as_int();
    } else if (name == "height") {
      camera->ny = integer.attribute("value").as_int();
    } else {
      std::cout << "Unknown integer attribute: " << name << std::endl;
      exit(-1);
    }
  }
}

void parseFilter(pugi::xml_node filter, Camera *camera) {
  std::string filter_type = filter.attribute("type").value();
  if (filter_type == "box") {
    camera->filter = std::make_unique<BoxFilter>(0.5);
  } else if (filter_type == "gaussian") {
    camera->filter = std::make_unique<GaussianFilter>(0.5);
  } else {
    std::cout << "Unknown filter: " << filter_type << std::endl;
    exit(-1);
  }
}

std::unique_ptr<Camera> parseSensor(pugi::xml_node sensor) {
  std::string sensor_type = sensor.attribute("type").value();
  if (sensor_type == "perspective") {
    std::unique_ptr<Camera> perspective_camera = std::make_unique<Camera>();
    Real fov;
    std::string fovAxis;
    for (pugi::xml_node float_node = sensor.child("float"); float_node;
         float_node = float_node.next_sibling("float")) {
      std::string name = float_node.attribute("name").value();
      // possible names: fov, fovAxis, nearClip, farClip, focalDistance
      // fovAxis , value = "smaller" means that use the smaller dimension for
      // fov fov uses degree these two are stored in temporary variables to
      // calculate the members of perspective_camera later others are directly
      // assigned to perspective_camera
      if (name == "fov") {
        fov = float_node.attribute("value").as_float();
      } else if (name == "fovAxis") {
        fovAxis = float_node.attribute("value").value();
      } else if (name == "nearClip") {
        perspective_camera->nearPlane =
            float_node.attribute("value").as_float();
      } else if (name == "farClip") {
        perspective_camera->farPlane = float_node.attribute("value").as_float();
      } else if (name == "focalDistance") {
        perspective_camera->focalDistance =
            float_node.attribute("value").as_float();
      } else {
        std::cout << "Unknown float attribute: " << name << std::endl;
        exit(-1);
      }
    }

    pugi::xml_node film = sensor.child("film");
    if (film) {
      parseFilm(film, perspective_camera.get());
    } else {
      std::cout << "No film node found" << std::endl;
      exit(-1);
    }
    pugi::xml_node rfilter = film.child("rfilter");
    if (rfilter) {
      parseFilter(rfilter, perspective_camera.get());
    } else {
      std::cout << "No rfilter node found" << std::endl;
      exit(-1);
    }
    pugi::xml_node sampler = sensor.child("sampler");
    if (sampler)
      perspective_camera->spp = sampler.attribute("sampleCount").as_int();
    else {
      std::cout << "No sampler node found" << std::endl;
      exit(-1);
    }
    if (sensor.child("transform")) {
      perspective_camera->transform = parseTransform(sensor.child("transform"));
    } else {
      std::cout << "No transform node found" << std::endl;
      exit(-1);
    }
    return std::move(perspective_camera);
  } else {
    std::cout << "Unknown sensor: " << sensor_type << std::endl;
    exit(-1);
  }
  // finally calculate the members of perspective_camera
  // TODO: implement this
}

Spectrum parseSpectrum(pugi::xml_node spectrum) {
  std::string spectrum_type = spectrum.attribute("type").value();
  if (spectrum_type == "uniform") {
    return Spectrum(spectrum.attribute("value").as_float());
  } else if (spectrum_type == "rgb") {
    return Spectrum(Vec3(spectrum.attribute("value").as_float()));
  } else {
    std::cout << "Unknown spectrum: " << spectrum_type << std::endl;
    exit(-1);
  }
}

void parseMaterial(pugi::xml_node bsdf) {
  std::string bsdf_type = bsdf.attribute("type").value();
  std::unique_ptr<Material> material;
  if (bsdf_type == "diffuse") {
    pugi::xml_node spectrum = bsdf.child("spectrum");
    if (spectrum) {
      material = std::make_unique<Lambertian>(parseSpectrum(spectrum));
    } else {
      std::cout << "No spectrum for diffuse" << std::endl;
      exit(-1);
    }
  }
  material_ref[bsdf.attribute("id").value()] = material.get();
  materials.emplace_back(material);
}

Spectrum parseSpectrum(const std::string &value) {
  std::vector<std::string> components = split(value, std::regex("(,| )+"));
  std::vector<std::tuple<int, Real>> distrib;
  for (auto &component : components) {
    // find ":" in component and put the two parts into distrib
    int pos = component.find(":");
    if (pos == std::string::npos) {
      std::cout << "Unknown spectrum format: " << value << std::endl;
      exit(-1);
    }
    distrib.push_back(std::make_tuple(std::stoi(component.substr(0, pos)),
                                      std::stod(component.substr(pos + 1))));
  }
  return AdaptiveSpectrum(distrib).toRGB();
}

std::unique_ptr<Shape> parseSphere(pugi::xml_node sphere) {
  Real radius;
  Transform* Obj2World = parseTransform(sphere.child("transform"));
  Transform* World2Obj = invTransform(Obj2World);
  radius = sphere.attribute("radius").as_float();
  return std::make_unique<Sphere>(radius, World2Obj, Obj2World);
}

std::shared_ptr<Light> parseLight(pugi::xml_node emitter, const std::shared_ptr<Shape>& shape) {
  std::string emitter_type = emitter.attribute("type").value();
  if (emitter_type == "area") {
    pugi::xml_node radiance = emitter.child("radiance");
    if (radiance) {
      return std::make_shared<AreaLight>(shape, parseSpectrum(radiance));
    } else {
      std::cout << "No radiance for area light" << std::endl;
      exit(-1);
    }
  } else {
    std::cout << "Unknown emitter: " << emitter_type << std::endl;
    exit(-1);
  }
}

// in Mitsuba's scene format it is called shape
// I follow PBRT's naming convention to call it primitive
void parsePrimitive(pugi::xml_node primitive) {
  if (primitive.attribute("type").empty()) {
    std::cout << "No type for shape" << std::endl;
    exit(-1);
  }
  std::string primitive_type = primitive.attribute("type").value();
  // if there is a ref child, then use the ref
  // it should only be a ref to a material
  // the id attribute can be used for looking up the material
  Material* material;
  if (primitive.child("ref")) {
    std::string ref = primitive.child("ref").attribute("id").value();
    if (material_ref.find(ref) == material_ref.end()) {
      std::cout << "Unknown material ref: " << ref << std::endl;
      exit(-1);
    }
    material = material_ref[ref];
  } else {
    // if there is no ref child, then use the bsdf child
    // use parseMaterial
    pugi::xml_node bsdf = primitive.child("bsdf");
    if (bsdf) {
      parseMaterial(bsdf);
      material = material_ref[bsdf.attribute("id").value()];
    } else {
      std::cout << "No bsdf for shape" << std::endl;
      exit(-1);
    }
  }
  // first take the emitter child
  // after we have the shapes, we parse the light and add it to the scene
  pugi::xml_node emitter = primitive.child("emitter");

  if (primitive_type == "sphere") {
    std::unique_ptr<Shape> sphere = parseSphere(primitive);
    primitives.emplace_back(
        std::make_unique<GeometryPrimitive>(sphere, material));
  } else if (primitive_type == "obj") {
    // find child of name "string", the "name" attribute should be "filename"
    // and the "value" attribute is the path relative to the file
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
    std::string filename;
    for (pugi::xml_node string_node = primitive.child("string"); string_node;
         string_node = string_node.next_sibling("string")) {
      std::string name = string_node.attribute("name").value();
      if (name == "filename") {
        filename = string_node.attribute("value").value();
      } else {
        std::cout << "Unknown string attribute: " << name << std::endl;
        exit(-1);
      }
    }
    loadObj(filename, mesh.get());
    // add all the triangles to the primitives
    for (auto &triangle : mesh->triangles)
      primitives.push_back(std::make_unique<GeometryPrimitive>(triangle, material));
  }
}

std::tuple<std::unique_ptr<Scene>, std::unique_ptr<Integrator>>
loadScene(const char *file_path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(file_path);
  // TODO: implement log lib to replace this
  std::cout << file_path << " parsing result: " << result.description()
            << std::endl;
  std::unique_ptr<Scene> scene = std::make_unique<Scene>();
  std::unique_ptr<Integrator> integrator = nullptr;
  pugi::xml_node root = doc.child("scene");
  if (root.empty()) {
    std::cout << "No scene node found in " << file_path << std::endl;
    exit(-1);
  }
  for (pugi::xml_node node : root.children()) {
    if (strcmp(node.name(), "integrator") == 0) {
      integrator = parseIntegrator(node);
    } else if (strcmp(node.name(), "sensor") == 0) {
      scene->camera = parseSensor(node);
    } else if (strcmp(node.name(), "background") == 0) {
      scene->envMap =
          std::make_unique<PureColor>(Vec3(node.attribute("color").as_float()));
    } else if (strcmp(node.name(), "bsdf") == 0) {
      parseMaterial(node);
    } else if (strcmp(node.name(), "shape") == 0) {
      parsePrimitive(node);
    } else {
      std::cout << "Unknown node: " << node.name() << std::endl;
      exit(-1);
    }
  }
  if (!integrator) {
    std::cout << "No integrator found in " << file_path
              << ", using default config." << std::endl;
    integrator = std::make_unique<PathTracer>();
  }
  scene->primitives = std::move(primitives);
  scene->materials = std::move(materials);
  scene->transforms = std::move(transforms);
  scene->pr = std::make_unique<Aggregate>(scene->primitives);
  return std::make_tuple(std::move(scene), std::move(integrator));
}
} // namespace rdcraft