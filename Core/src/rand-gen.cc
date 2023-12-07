//
// Created by creeper on 9/21/23.
//
#include <Core/rand-gen.h>
#include <random>
namespace rdcraft {
Real randomReal() {
  // return a random number in [0, 1)
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis(0, 1);
  return dis(gen);
}
Real randomReal(Real l, Real r) { return randomReal() * (r - l) + l; }
int randomInt(int l, int r) {
  // return a random number in [l, r)
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(l, r - 1);
  return dis(gen);
}
} // namespace rdcraft
