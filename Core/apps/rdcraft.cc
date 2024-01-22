//
// Created by creeper on 11/9/23.
//
#include <Core/scene.h>
#include <Core/scene-parser.h>
#include <Core/integrators.h>
#include <cstdio>
#ifndef USE_HARD_CODED_SCENE
using rdcraft::loadScene;
#endif
using rdcraft::Integrator;
using rdcraft::Scene;
int main(int argc, char** argv) {
  std::printf("RenderCraft version 1.0.0\n");
#ifndef USE_HARD_CODED_SCENE
  if (argc < 2) {
    std::printf("Usage: rdcraft <path_to_scene_description_file>\n");
    return 0;
  }
  const char* scene_path = argv[1];
  printf("Parsing scene desciption file: %s\n", scene_path);
  auto [scene, integrator] = loadScene(scene_path);
  printf("Parsing done.\n");
#else
  auto [scene, integrator] = rdcraft::hardCodedScene();
  printf("Scene constructed.\n");
#endif
  integrator->render(scene.get());
  printf("Bye-bye!\n");
}