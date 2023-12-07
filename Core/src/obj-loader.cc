#include <Core/obj-loader.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace rdcraft {
#ifndef USE_TINYOBJLOADER
bool loadObj(const std::string &path, Mesh *mesh) {
  std::unordered_map<int, Real> sum_adj_area;
  std::unordered_map<int, Vec3> sum_normal;
  std::ifstream file(path);
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;
    if (type == "v") {
      Real x, y, z;
      iss >> x >> y >> z;
      mesh->vertices.emplace_back(x - 1, y - 1, z - 1);
    } else if (type == "vt") {
      Real u, v;
      iss >> u >> v;
      mesh->uvs.emplace_back(u - 1, v - 1);
    } else if (type == "vn") {
      Real x, y, z;
      iss >> x >> y >> z;
      mesh->normals.emplace_back(x - 1, y - 1, z - 1);
    } else if (type == "f") {
      std::string face;
      int v_idx[3], vt_idx[3] = {-1, -1, -1}, vn_idx[3] = {-1, -1, -1};
      for (int i = 0; i < 3; ++i) {
        iss >> v_idx[i];
        if (iss.peek() == '/') {
          iss.ignore();
          if (iss.peek() != '/') {
            iss >> vt_idx[i];
          }
          if (iss.peek() == '/') {
            iss.ignore();
            iss >> vn_idx[i];
          }
        }
      }
      Vec3 normal = cross(mesh->vertices[v_idx[1]] - mesh->vertices[v_idx[0]],
                          mesh->vertices[v_idx[2]] - mesh->vertices[v_idx[0]]);
      for (int i = 0; i < 3; i++) {
        if (vn_idx[i] >= 0) continue;
        sum_adj_area[v_idx[i]] += normal.length();
        sum_normal[v_idx[i]] += normal;
      }
      Triangle triangle;
      triangle.v[0] = v_idx[0];
      triangle.v[1] = v_idx[1];
      triangle.v[2] = v_idx[2];
      triangle.n[0] = vn_idx[0];
      triangle.n[1] = vn_idx[1];
      triangle.n[2] = vn_idx[2];
      triangle.uv[0] = vt_idx[0];
      triangle.uv[1] = vt_idx[1];
      triangle.uv[2] = vt_idx[2];
      mesh->triangles.emplace_back(triangle);
    }
  }
  mesh->triangleCount = mesh->triangles.size();
  file.close();
}
#else
bool loadObj(const std::string &path, Mesh *mesh) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  std::string warn;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

  if (!warn.empty())
    std::cerr << "[TINYOBJLOADER] Warning: " << warn << std::endl;

  if (!err.empty()) {
    std::cerr << err << std::endl;
    return false;
  }
  if (!ret) {
    std::cerr << "[TINYOBJLOADER] Failed to load " << path << std::endl;
    return false;
  }

  mesh->triangleCount = 0;
  for (const auto& shape : shapes) {
    for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
      std::array<int, 3> v;
      std::array<int, 3> n;
      std::array<int, 3> tex;
      for (int i = 0; i < 3; i++) {
        int idx = shape.mesh.indices[3 * f + i].vertex_index;
        int normal_idx = shape.mesh.indices[3 * f + i].normal_index;
        int uv_idx = shape.mesh.indices[3 * f + i].texcoord_index;

        Vec3 vertex(attrib.vertices[3 * idx + 0], attrib.vertices[3 * idx + 1], attrib.vertices[3 * idx + 2]);
        Vec3 normal(attrib.normals[3 * normal_idx + 0], attrib.normals[3 * normal_idx + 1], attrib.normals[3 * normal_idx + 2]);
        Vec2 uv(attrib.texcoords[2 * uv_idx + 0], attrib.texcoords[2 * uv_idx + 1]);

        mesh->vertices.emplace_back(attrib.vertices[3 * idx + 0], attrib.vertices[3 * idx + 1], attrib.vertices[3 * idx + 2]);
        mesh->normals.emplace_back(attrib.normals[3 * normal_idx + 0], attrib.normals[3 * normal_idx + 1], attrib.normals[3 * normal_idx + 2]);
        mesh->uvs.emplace_back(attrib.texcoords[2 * uv_idx + 0], attrib.texcoords[2 * uv_idx + 1]);
        v[i] = idx;
        n[i] = normal_idx;
        tex[i] = uv_idx;
      }
      mesh->triangleCount++;
      mesh->triangles.emplace_back();
    }
  }
  return false;
}
#endif
} // namespace rdcraft