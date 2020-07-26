//
// Created by Brian Landers on 2019-01-10.
//

#pragma once
#include <ostream>

#include "ray.h"
#include "tuple.h"

class Sphere;

class Intersection {
 public:
  Intersection(double t, Sphere *o) : t_(t), sphere_(o) {}
  double t() const { return t_; };

  Sphere *object() const { return sphere_; }

  friend std::ostream &operator<<(std::ostream &os, const Intersection &rhs) {
    return os << "Intersection(" << rhs.t() << ")";
  }

 private:
  double t_;
  Sphere *sphere_;
};

inline bool operator==(const Intersection &a, const Intersection &b) {
  return a.t() == b.t() && a.object() == b.object();
}
