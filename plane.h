//
// Created by Brian Landers on 7/27/20.
//
#pragma once

#include "shape.h"

class Plane : public Shape {
  bool compare(const Shape&) const noexcept override { return true; }
  std::vector<Intersection> local_intersect(const Ray& r) override {
    std::vector<Intersection> out;

    if (abs(r.direction().y) < EPSILON) {
      return out;
    }
    auto t = -r.origin().y / r.direction().y;
    return {Intersection(t, shared_from_this())};
  };

  Tuple local_normal_at(const Tuple& p) override {
    return Tuple::vector(0, 1, 0);
  }
};
