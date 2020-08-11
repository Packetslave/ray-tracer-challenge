//
// Created by Brian Landers on 7/27/20.
//
#pragma once

#include "shape.h"

class Plane : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }
  IntersectionVector local_intersect(const Ray& r) override {
    IntersectionVector out;

    if (abs(r.direction().y) < EPSILON) {
      return out;
    }
    auto t = -r.origin().y / r.direction().y;
    return {Intersection(t, this)};
  };

  Tuple local_normal_at(const Tuple& p) override {
    return Tuple::vector(0, 1, 0);
  }

  BoundingBox* bounds_of() override { return &box_; }

 private:
  BoundingBox box_ = { Tuple::point(-DBL_MAX, 0, -DBL_MAX), Tuple::point(DBL_MAX, 0, DBL_MAX) };
};
