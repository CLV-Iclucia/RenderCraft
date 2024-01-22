// add protection against multiple inclusions
// use full path for protection
#ifndef RENDERCRAFT_OBJ_LOADER_H
#define RENDERCRAFT_OBJ_LOADER_H
#include <Core/mesh.h>
#include <string>
#define USE_MODIFIED_LAJOLLA_LOADER
namespace rdcraft {

#ifdef USE_MODIFIED_LAJOLLA_LOADER
bool loadObj(const std::string& filename, const Mat4& to_world, Mesh* mesh);
#else
bool loadObj(const std::string& filename, Mesh* mesh);
#endif
}

#endif
