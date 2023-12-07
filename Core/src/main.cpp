#include <Core/Scene.h>
#include <Core/scene-parser.h>
#include <cstdio>
#include <Core/rdcraft.h>
using rdcraft::loadScene;
using rdcraft::Integrator;
using rdcraft::Scene;
int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("RenderCraft version 1.0.0\n");
    return 0;
  }
  const char* scene_path = argv[1];
  printf("Parsing scene desciption file: %s\n", scene_path);
  auto [scene, integrator] = loadScene(scene_path);
  printf("Parsing done.\n");
  integrator->render(scene.get());
}