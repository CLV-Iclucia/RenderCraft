#include <Core/Scene.h>
#include <Core/scene-parser.h>
#include <stdio.h>
#include <iostream>
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
  auto [scene, integrator] = loadScene(scene_path);
}