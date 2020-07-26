#pragma once

#include "matrix.h"
#include "tuple.h"

class Ray {
 public:
  Ray(const Tuple origin, const Tuple direction)
      : origin_(origin), direction_(direction) {}

  Tuple origin() const { return origin_; }

  Tuple direction() const { return direction_; }

  Tuple position(const float distance) const {
    return origin_ + (direction_ * distance);
  }

  Ray transform(const Matrix& m) const {
    return Ray(m * origin_, m * direction_);
  }

 private:
  Tuple origin_;
  Tuple direction_;
};
