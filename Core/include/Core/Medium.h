//
// Created by creeper on 23-4-12.
//

#ifndef RENDERCRAFT_MEDIUM_H
#define RENDERCRAFT_MEDIUM_H

struct MediumInterface {
  int internal_id = -1;
  int external_id = -1;
  MediumInterface(int internal, int external) : internal_id(internal), external_id(external) {}
};
#endif //RENDERCRAFT_MEDIUM_H
