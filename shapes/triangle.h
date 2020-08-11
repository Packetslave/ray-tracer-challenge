//
// Created by Brian Landers on 8/8/20.
//

#include "../core/bounding_box.h"
#include "shape.h"

#pragma once

class Triangle : public Shape {
 public:
  Triangle(const Tuple& p1, const Tuple& p2, const Tuple& p3)
      : Shape(),
        p1{p1},
        p2{p2},
        p3{p3},
        e1{p2 - p1},
        e2{p3 - p1},
        normal{cross(e2, e1).normalize()} {
    box_.add(p1);
    box_.add(p2);
    box_.add(p3);
  }

  bool compare(const Shape&) const noexcept override { return true; }

  IntersectionVector local_intersect(const Ray& r) override  {
    auto dir_cross_e2 = cross(r.direction(), e2);
    auto det = dot(e1, dir_cross_e2);
    if (abs(det) < EPSILON) {
      return {};
    }
    auto f = 1.0 / det;
    auto p1_to_origin = r.origin() - p1;
    auto u = f * dot(p1_to_origin, dir_cross_e2);
    if (u < 0 || u > 1) {
      return {};
    }

    auto origin_cross_e1 = cross(p1_to_origin, e1);
    auto v = f * dot(r.direction(), origin_cross_e1);
    if (v < 0 || (u + v) > 1) {
      return {};
    }

    auto t = f * dot(e2, origin_cross_e1);
    return { Intersection(t, this) };
  };

  Tuple local_normal_at(const Tuple& p) override { return normal; }

  BoundingBox* bounds_of() override {
    return &box_;
  }

  Tuple p1, p2, p3;
  Tuple e1, e2, normal;
  BoundingBox box_;
};
