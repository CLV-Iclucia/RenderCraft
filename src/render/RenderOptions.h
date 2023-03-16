//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_RENDEROPTIONS_H
#define RENDERCRAFT_RENDEROPTIONS_H
#include "types.h"

struct RenderOptions
{
    const uint spp = 256;
    const Real PRR = 0.95;
    const uint ScrWid = 1024, ScrHeight = 1024;
    std::string savingPath = "./output.ppm";
    bool enableProcessOutputs = true;
};
#endif //RENDERCRAFT_RENDEROPTIONS_H
