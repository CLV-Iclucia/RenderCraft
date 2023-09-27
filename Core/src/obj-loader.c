#include <Core/obj-loader.h>
#include <fstream>
#include <iostream>
#include <sstream>
namespace rdcraft {
bool loadObj(const std::string &path, Mesh *mesh) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << path << std::endl;
    return false;
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string token;
      iss >> token;
      if (token == "v") { // Vertex
        Vec3 vertex; 
        iss >> vertex.x >> vertex.y >> vertex.z;
        mesh->vertices.push_back(vertex);
      } else if (token == "f") { // Face
        Triangle triangle;
        iss >> triangle.v[0] >> triangle.v[1] >> triangle.v[2];
        face.x -= 1;
        face.y -= 1;
        face.z -= 1;

      }
    }
    file.close();
    return true;
  }
}
} // namespace rdcraft