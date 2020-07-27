#pragma once

#include <ostream>

#include "tuple.h"

class Color : public Tuple {
 public:
  Color(double red, double green, double blue) : Tuple(red, green, blue, 0.0) {}
  Color(const Tuple& a) : Tuple(a.x, a.y, a.z, 0.0) {}

  [[nodiscard]] double r() const { return x; }
  [[nodiscard]] double g() const { return y; }
  [[nodiscard]] double b() const { return z; }

  friend std::ostream& operator<<(std::ostream& os, const Color& rhs) {
    return os << "Color(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
  }

  Color& operator=(Tuple other) {
    std::swap(x, other.x);
    std::swap(y, other.y);
    std::swap(z, other.z);
    return *this;
  }

  Color& operator*=(const Color rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;

    return *this;
  }

  friend Color operator*(Color lhs, const Color& rhs) {
    lhs *= rhs;
    return lhs;
  }
};
