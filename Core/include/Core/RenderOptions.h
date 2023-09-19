//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_RENDEROPTIONS_H
#define RENDERCRAFT_RENDEROPTIONS_H
#include "core.h"

struct RenderOptions
{
    const uint spp = 256;
    const Real PRR = 0.95;
    const Real scrWid = 4, scrHeight = 4, scrZ = -1;
    std::string savingPath = "./output.ppm";
    bool enableDisplayProcess = true; ///< whether to display the rendering process
    bool enableLogOutputs = false; ///< whether to output the rendering log
};
#endif //RENDERCRAFT_RENDEROPTIONS_H
