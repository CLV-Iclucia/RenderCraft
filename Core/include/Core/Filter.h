//
// Created by creeper on 23-3-16.
//

#ifndef RENDERCRAFT_FILTER_H
#define RENDERCRAFT_FILTER_H

#include <Core/core.h>
#include <Core/rand-gen.h>
namespace rdcraft {
class Filter {
 public:
  virtual Vec2 sample() const = 0;
  virtual Real pdfSample(Real x, Real y) const = 0;
};

class BoxFilter : public Filter {
 private:
  Real window_x = 0.0, window_y = 0.0;
 public:
  Vec2 sample() const override {
    return {get_random() * window_x - window_x * 0.5, get_random() * window_y - window_y * 0.5};
  }
  Real pdfSample(Real x, Real y) const override {
    return 1.0 / window_x / window_y;
  }
};

class GaussianFilter : public Filter {

};
}
#endif //RENDERCRAFT_FILTER_H
