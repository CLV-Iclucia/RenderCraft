#ifndef RENDERCRAFT_MESH_H
#define RENDERCRAFT_MESH_H
#include <Core/Triangle.h>
#include <Core/core.h>
#include <vector>
namespace rdcraft {
struct Mesh {
    int triangleCount;
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;
    std::vector<Triangle> triangles;
};
}

#endif