#ifndef RENDERCRAFT_CORE_INCLUDE_CORE_SCENE_PARSER_H_
#define RENDERCRAFT_CORE_INCLUDE_CORE_SCENE_PARSER_H_
// for now I shall use pugixml, in the future I will use my own xml parser instead
#include <memory>
#include <pugixml.hpp>
#include <Core/Scene.h>
#include <tuple>
namespace rdcraft {
std::tuple<std::unique_ptr<Scene>, std::unique_ptr<Integrator>> loadScene(const char* file_path);
}
#endif
