//
// Created by creeper on 9/21/23.
//
#include "Core/Primitive.h"
#include "Core/Spectrums.h"
#include <Core/Integrator.h>
#include <Core/Sphere.h>
#include <Core/scene-parser.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <Core/Material.h>
#include <map>
namespace rdcraft {

static std::map<std::string, std::shared_ptr<Material>> material_ref;
static std::vector<std::shared_ptr<Primitive>> primitives;
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
      } else if (name == "spp") {
        path_tracer->setSamplesPerPixel(integer.attribute("value").as_int());
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

std::unique_ptr<Camera> parseSensor(pugi::xml_node sensor) {
  std::string sensor_type = sensor.attribute("type").value();
  if (sensor_type == "perspective") {
    std::unique_ptr<Camera> perspective_camera =
        std::make_unique<Camera>();
    for (pugi::xml_node float_node = sensor.child("float"); float_node;
         float_node = float_node.next_sibling("float")) {
      std::string name = float_node.attribute("name").value();
      if (name == "fov") {
      } else if (name == "aspect") {
      } else {
        std::cout << "Unknown float attribute: " << name << std::endl;
        exit(-1);
      }
    }
    return std::move(perspective_camera);
  } else {
    std::cout << "Unknown sensor: " << sensor_type << std::endl;
    exit(-1);
  }
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
  std::shared_ptr<Material> material;
  if (bsdf_type == "diffuse") {
    pugi::xml_node spectrum = bsdf.child("spectrum");
    if (spectrum) {
      material = std::make_shared<Lambertian>(parseSpectrum(spectrum));
    } else {
      std::cout << "No spectrum for diffuse" << std::endl;
      exit(-1);
    }
  }
  material_ref[bsdf.attribute("id").value()] = material;
}

// in Mitsuba's scene format it is called shape
// I follow PBRT's naming convention to call it primitive
void parsePrimitive(pugi::xml_node primitive) {
  std::string primitive_type = primitive.attribute("type").value();
  if (primitive_type == "sphere") {
    pugi::xml_node bsdf = primitive.child("bsdf");
    if (bsdf) {
      parseMaterial(bsdf);
    } else {
      std::cout << "No bsdf for sphere" << std::endl;
      exit(-1);
    }
    pugi::xml_node transform = primitive.child("transform");
    if (transform) {
      std::cout << "Transform not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node float_node = primitive.child("float");
    if (float_node) {
      std::cout << "Float not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node integer = primitive.child("integer");
    if (integer) {
      std::cout << "Integer not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node string = primitive.child("string");
    if (string) {
      std::cout << "String not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node point = primitive.child("point");
    if (point) {
      std::cout << "Point not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node vector = primitive.child("vector");
    if (vector) {
      std::cout << "Vector not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node normal = primitive.child("normal");
    if (normal) {
      std::cout << "Normal not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node texture = primitive.child("texture");
    if (texture) {
      std::cout << "Texture not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node boolean = primitive.child("boolean");
    if (boolean) {
      std::cout << "Boolean not implemented" << std::endl;
      exit(-1);
    }
    pugi::xml_node matrix = primitive.child("matrix");
    if (matrix) {
      std::cout << "Matrix not implemented" << std::endl;
      exit(-1);
    }
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
  std::unique_ptr<Integrator> integrator;
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
  return std::make_tuple(std::move(scene), std::move(integrator));
}
} // namespace rdcraft