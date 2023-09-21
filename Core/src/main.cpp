#include <stdio.h>
#include <iostream>
#include <Core/rdcraft.h>
using namespace rdcraft;
int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("RenderCraft version 1.0.0\n");
    return 0;
  }
  const char* scene_path = argv[0];
  Integrator* integrator = nullptr;
}