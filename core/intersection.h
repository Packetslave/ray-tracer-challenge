//
// Created by Brian Landers on 2019-01-10.
//

#pragma once
#include <ostream>
#include <boost/flyweight.hpp>
#include <folly/small_vector.h>
#include <folly/Traits.h>
#include <folly/FBVector.h>

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

//namespace folly {
//  template<>
//  struct IsRelocatable<Intersection> : boost::true_type {};
//}

//using IntersectionVector = folly::small_vector<Intersection, 2>;
using IntersectionVector = std::vector<Intersection>;
//using IntersectionVector = folly::fbvector<Intersection>;