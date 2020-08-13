//
// Created by Brian Landers on 2019-01-10.
//

#pragma once
#include <folly/FBVector.h>
#include <folly/Traits.h>
#include <folly/memory/Arena.h>
#include <folly/small_vector.h>

#include <boost/flyweight.hpp>
#include <ostream>

#include "ray.h"
#include "tuple.h"
#include <tbb/scalable_allocator.h>

class Shape;
class Intersection {
 public:
  Intersection(double t, Shape *o) : t_(t), shape_(o) {}
  Intersection(double t, Shape *o, double u, double v)
      : t_(t), shape_(o), u{u}, v{v} {}
  double t() const { return t_; };

  Shape *object() const { return shape_; }

  friend std::ostream &operator<<(std::ostream &os, const Intersection &rhs) {
    return os << "Intersection(" << rhs.t() << ")";
  }

  double u, v;

 private:
  double t_;
  Shape *shape_;
};

inline bool operator==(const Intersection &a, const Intersection &b) {
  return a.t() == b.t() && a.object() == b.object();
}

// namespace folly {
//  template<>
//  struct IsRelocatable<Intersection> : boost::true_type {};
//}

//using IntersectionVector = folly::small_vector<Intersection, 4>;
 using IntersectionVector = std::vector<Intersection, tbb::scalable_allocator<Intersection>>;
// using IntersectionVector = folly::fbvector<Intersection>;
// using IntersectionVector = std::vector<Intersection,
// folly::SysArenaAllocator<Intersection>>;