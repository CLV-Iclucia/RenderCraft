#include <Core/obj-loader.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
namespace rdcraft {
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
} // namespace rdcraft