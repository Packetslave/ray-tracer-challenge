#pragma once

#include <optional>
#include <vector>

#include "../core/intersection.h"
#include "../core/material.h"
#include "../core/ray.h"
#include "folly/Optional.h"
#include "shape.h"

class Sphere : public Shape {
 public:
  static std::shared_ptr<Shape> Glass() {
    auto out = std::shared_ptr<Shape>();
    out.reset(new Sphere());
    out->material()->set_transparency(1.0);
    out->material()->set_refractive(1.5);
    return out;
  }

  explicit Sphere() : Shape() {}

  // FIXME: this can be optimized a fair bit
  IntersectionVector local_intersect(const Ray &r) override {
    auto sphere_to_ray = r.origin() - Tuple::point(0, 0, 0);
    auto a = dot(r.direction(), r.direction());
    auto b = 2 * dot(r.direction(), sphere_to_ray);
    auto c = dot(sphere_to_ray, sphere_to_ray) - 1;
    auto d = b * b - 4 * a * c;

    if (d < 0) {
      return IntersectionVector();
    }

    IntersectionVector out{
        Intersection((-b - sqrt(d)) / (2 * a), this),
        Intersection((-b + sqrt(d)) / (2 * a), this),
    };
    return out;
  }

  Tuple local_normal_at(const Tuple &p, const Intersection* i) override {
    auto normal = p - Tuple::point(0, 0, 0);
    // normal.w = 0;
    return normal;
  }

  BoundingBox* bounds_of() override {
    return &box_;
  }

  bool compare(const Shape &) const noexcept override { return true; }

 private:
  BoundingBox box_ = { Tuple::point(-1, -1, -1), Tuple::point(1, 1, 1) };
};

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  if (!v.empty()) {
    out << '[';
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}

std::optional<Intersection> Hit(const IntersectionVector &v);