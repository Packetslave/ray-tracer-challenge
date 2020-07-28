//
// Created by Brian Landers on 2019-01-10.
//

#pragma once
#include <ostream>

#include "ray.h"
#include "tuple.h"

class Shape;

class Intersection {
 public:
  Intersection(double t, std::shared_ptr<Shape> o) : t_(t), shape_(o) {}
  double t() const { return t_; };

  std::shared_ptr<Shape> object() const { return shape_; }

  friend std::ostream &operator<<(std::ostream &os, const Intersection &rhs) {
    return os << "Intersection(" << rhs.t() << ")";
  }

 private:
  double t_;
  std::shared_ptr<Shape> shape_;
};

inline bool operator==(const Intersection &a, const Intersection &b) {
  return a.t() == b.t() && a.object() == b.object();
}
