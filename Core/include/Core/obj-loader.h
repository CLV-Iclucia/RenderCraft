// add protection against multiple inclusions
// use full path for protection
#ifndef RENDERCRAFT_OBJ_LOADER_H
#define RENDERCRAFT_OBJ_LOADER_H
#include <Core/mesh.h>
#include <string>
#define USE_TINYOBJLOADER
#ifdef USE_TINYOBJLOADER
#include <tiny_obj_loader.h>
#endif
namespace rdcraft {
bool loadObj(const std::string& path, Mesh* mesh);
}
#endif
