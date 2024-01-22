#include <Core/obj-loader.h>
#include <Core/utils.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <map>
#include <sstream>
#include <unordered_map>

namespace rdcraft {
#ifndef USE_MODIFIED_LAJOLLA_LOADER

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
// the following code is slightly modified from the obj loader in lajolla by Tzumao Li
// because all my test scenes are directly taken from lajolla

// https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start
static inline std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::ranges::find_if(s, [](unsigned char ch) {
    return !std::isspace(ch);
  }));
  return s;
}
// trim from end
static inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
  return s;
}
// trim from both ends
static inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

static std::vector<int> split_face_str(const std::string& s) {
  std::vector<int> result;

  std::istringstream iss(s);
  std::string token;

  while (std::getline(iss, token, '/')) {
    if (!token.empty()) {
      result.push_back(std::stoi(token));
    } else {
      result.push_back(0);
    }
  }

  while (result.size() < 3) {
    result.push_back(0);
  }

  return result;
}

// Numerical robust computation of angle between unit vectors
inline Real unit_angle(const Vec3& u, const Vec3& v) {
  if (dot(u, v) < 0)
    return (PI - 2) * asin(Real(0.5) * length(v + u));
  else
    return 2 * asin(Real(0.5) * length(v - u));
}

inline std::vector<Vec3> compute_normal(const std::vector<Vec3>& vertices,
                                        const std::vector<int>& indices) {
  std::vector<Vec3> normals(vertices.size(), Vec3{0, 0, 0});

  // Nelson Max, "Computing Vertex Normals from Facet Normals", 1999
  CHECK_EQZ(indices.size() % 3);
  for (int index = 0; index < indices.size() / 3; index++) {
    Vec3 n = Vec3{0, 0, 0};
    for (int i = 0; i < 3; ++i) {
      const Vec3& v0 = vertices[index + i];
      const Vec3& v1 = vertices[index + ((i + 1) % 3)];
      const Vec3& v2 = vertices[index + ((i + 2) % 3)];
      Vec3 side1 = v1 - v0, side2 = v2 - v0;
      if (i == 0) {
        n = cross(side1, side2);
        Real l = length(n);
        if (l == 0) {
          break;
        }
        n = n / l;
      }
      Real angle = unit_angle(normalize(side1), normalize(side2));
      normals[index + i] = normals[index + i] + n * angle;
    }
  }

  for (auto& n : normals) {
    Real l = length(n);
    if (l != 0) {
      n = n / l;
    } else {
      // degenerate normals, set it to 0
      n = Vec3{0, 0, 0};
    }
  }
  return normals;
}

struct ObjVertex {
  ObjVertex(const std::vector<int>& id)
    : v(id[0] - 1), vt(id[1] - 1), vn(id[2] - 1) {
  }

  bool operator<(const ObjVertex& vertex) const {
    if (v != vertex.v) {
      return v < vertex.v;
    }
    if (vt != vertex.vt) {
      return vt < vertex.vt;
    }
    if (vn != vertex.vn) {
      return vn < vertex.vn;
    }
    return false;
  }

  int v, vt, vn;
};

static Vec3 xform_point(const Mat4& xform, const Vec3& p) {
  return Vec3{
      xform[0][0] * p.x + xform[0][1] * p.y + xform[0][2] * p.z + xform[0][3],
      xform[1][0] * p.x + xform[1][1] * p.y + xform[1][2] * p.z + xform[1][3],
      xform[2][0] * p.x + xform[2][1] * p.y + xform[2][2] * p.z + xform[2][3]};
}

static Vec3 xform_normal(const Mat4& inv_xform, const Vec3& n) {
  return Vec3{
      inv_xform[0][0] * n.x + inv_xform[1][0] * n.y + inv_xform[2][0] * n.z,
      inv_xform[0][1] * n.x + inv_xform[1][1] * n.y + inv_xform[2][1] * n.z,
      inv_xform[0][2] * n.x + inv_xform[1][2] * n.y + inv_xform[2][2] * n.z};
}

size_t get_vertex_id(const ObjVertex& vertex,
                     const std::vector<Vec3>& pos_pool,
                     const std::vector<Vec2>& st_pool,
                     const std::vector<Vec3>& nor_pool,
                     const Mat4& to_world,
                     std::vector<Vec3>& pos,
                     std::vector<Vec2>& st,
                     std::vector<Vec3>& nor,
                     std::map<ObjVertex, size_t>& vertex_map) {
  auto it = vertex_map.find(vertex);
  if (it != vertex_map.end()) {
    return it->second;
  }
  size_t id = pos.size();
  pos.push_back(xform_point(to_world, pos_pool[vertex.v]));
  if (vertex.vt != -1)
    st.push_back(st_pool[vertex.vt]);
  if (vertex.vn != -1) {
    nor.push_back(xform_normal(inverse(to_world), nor_pool[vertex.vn]));
  }
  vertex_map[vertex] = id;
  return id;
}

bool loadObj(const std::string& filename, const Mat4& to_world, Mesh* mesh) {
  std::vector<Vec3> pos_pool;
  std::vector<Vec3> nor_pool;
  std::vector<Vec2> st_pool;
  std::map<ObjVertex, size_t> vertex_map;

  std::ifstream ifs(filename, std::ifstream::in);
  if (!ifs.is_open())
    ERROR("Unable to open the obj file");
  while (ifs.good()) {
    std::string line;
    std::getline(ifs, line);
    line = trim(line);
    if (line.size() == 0 || line[0] == '#') {
      // comment
      continue;
    }

    std::stringstream ss(line);
    std::string token;
    ss >> token;
    if (token == "v") {
      // vertices
      Real x, y, z, w = 1;
      ss >> x >> y >> z >> w;
      pos_pool.push_back(Vec3{x, y, z} / w);
    } else if (token == "vt") {
      Real s, t, w;
      ss >> s >> t >> w;
      st_pool.emplace_back(s, 1 - t);
    } else if (token == "vn") {
      Real x, y, z;
      ss >> x >> y >> z;
      nor_pool.push_back(normalize(Vec3{x, y, z}));
    } else if (token == "f") {
      std::string i0, i1, i2;
      ss >> i0 >> i1 >> i2;
      std::vector<int> i0f = split_face_str(i0);
      std::vector<int> i1f = split_face_str(i1);
      std::vector<int> i2f = split_face_str(i2);

      ObjVertex v0(i0f), v1(i1f), v2(i2f);
      size_t v0id = get_vertex_id(v0,
                                  pos_pool,
                                  st_pool,
                                  nor_pool,
                                  to_world,
                                  mesh->vertices,
                                  mesh->uvs,
                                  mesh->normals,
                                  vertex_map);
      size_t v1id = get_vertex_id(v1,
                                  pos_pool,
                                  st_pool,
                                  nor_pool,
                                  to_world,
                                  mesh->vertices,
                                  mesh->uvs,
                                  mesh->normals,
                                  vertex_map);
      size_t v2id = get_vertex_id(v2,
                                  pos_pool,
                                  st_pool,
                                  nor_pool,
                                  to_world,
                                  mesh->vertices,
                                  mesh->uvs,
                                  mesh->normals,
                                  vertex_map);
      mesh->indices.emplace_back(v0id);
      mesh->indices.emplace_back(v1id);
      mesh->indices.emplace_back(v2id);

      std::string i3;
      if (ss >> i3) {
        std::vector<int> i3f = split_face_str(i3);
        ObjVertex v3(i3f);
        size_t v3id = get_vertex_id(v3,
                                    pos_pool,
                                    st_pool,
                                    nor_pool,
                                    to_world,
                                    mesh->vertices,
                                    mesh->uvs,
                                    mesh->normals,
                                    vertex_map);
        mesh->indices.emplace_back(v0id);
        mesh->indices.emplace_back(v1id);
        mesh->indices.emplace_back(v2id);
      }
      std::string i4;
      if (ss >> i4)
        ERROR("The object file contains n-gon (n>4) that we do not support.");
    } // Currently ignore other tokens
  }
  if (mesh->normals.size() == 0) {
    mesh->normals = compute_normal(mesh->vertices, mesh->indices);
  }
  mesh->triangleCount = mesh->indices.size() / 3;
  return mesh;
}

#endif
} // namespace rdcraft