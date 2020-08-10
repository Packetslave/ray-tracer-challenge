//
// Created by Brian Landers on 8/7/20.
//

#ifndef RAY_TRACING2_CUBE_H
#define RAY_TRACING2_CUBE_H

#include "shape.h"

class Cube : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }
  IntersectionVector local_intersect(const Ray& r) override {
    auto [ytmin, ytmax] = check_axis(r.origin().y, r.direction().y);
    auto [ztmin, ztmax] = check_axis(r.origin().z, r.direction().z);
    auto [xtmin, xtmax] = check_axis(r.origin().x, r.direction().x);

    auto tmin = std::max({xtmin, ytmin, ztmin});
    auto tmax = std::min({xtmax, ytmax, ztmax});

    if (tmin > tmax) {
      return {};
    }
    return { Intersection(tmin, shared_from_this()), Intersection(tmax, shared_from_this())};
  };

  Tuple local_normal_at(const Tuple& p) override {
    auto maxc = std::max({abs(p.x), abs(p.y), abs(p.z)});
    if (maxc == abs(p.x)) {
      return Tuple::vector(p.x, 0, 0);
    } else if (maxc == abs(p.y)) {
      return Tuple::vector(0, p.y, 0);
    } else {
      return Tuple::vector(0, 0, p.z);
    }
  }

  std::pair<double, double> check_axis(const double origin, const double direction) {
    auto tmin_num = (-1 - origin);
    auto tmax_num = (1 - origin);

    double tmin, tmax;
    if (abs(direction) >= EPSILON) {
      tmin = tmin_num / direction;
      tmax = tmax_num / direction;
    } else {
      tmin = tmin_num * INFINITY;
      tmax = tmax_num * INFINITY;
    }
    if (tmin > tmax) {
      auto tmp = tmin;
      tmin = tmax;
      tmax = tmp;
    }
    return {tmin, tmax};
  }
};

#endif  // RAY_TRACING2_CUBE_H
