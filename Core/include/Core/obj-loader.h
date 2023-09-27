// add protection against multiple inclusions
// use full path for protection
#ifndef RENDERCRAFT_OBJ_LOADER_H
#define RENDERCRAFT_OBJ_LOADER_H
#include <Core/Mesh.h>
#include <string>
namespace rdcraft {
bool loadObj(const std::string& path, Mesh* mesh);
}
#endif
