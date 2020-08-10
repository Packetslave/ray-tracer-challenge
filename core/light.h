#pragma once

#include "color.h"
#include "tuple.h"

class PointLight {
 public:
  PointLight(Tuple position, Color intensity)
      : intensity_(intensity), position_(position) {}

  Color intensity() { return intensity_; }
  Tuple position() { return position_; }

  bool operator==(const PointLight& rhs) const {
    return this->intensity_ == rhs.intensity_;
  }

 private:
  Color intensity_;
  Tuple position_;
};
