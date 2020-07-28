#pragma once

#include <optional>
#include <vector>

#include "intersection.h"
#include "material.h"
#include "ray.h"
#include "shape.h"
#include "folly/Optional.h"

class Sphere : public Shape {
 public:
  static std::shared_ptr<Shape> Glass() {
    auto out = std::shared_ptr<Shape>();
    out.reset( new Sphere() );
    out->material()->set_transparency(1.0);
    out->material()->set_refractive(1.5);
    return out;
  }

  explicit Sphere() : Shape() {}

  // FIXME: this can be optimized a fair bit
  std::vector<Intersection> local_intersect(const Ray &r) override {
    auto sphere_to_ray = r.origin() - Tuple::point(0, 0, 0);
    auto a = dot(r.direction(), r.direction());
    auto b = 2 * dot(r.direction(), sphere_to_ray);
    auto c = dot(sphere_to_ray, sphere_to_ray) - 1;
    auto d = b * b - 4 * a * c;

    if (d < 0) {
      return std::vector<Intersection>();
    }

    std::vector<Intersection> out{Intersection((-b - sqrt(d)) / (2 * a), shared_from_this()),
                                  Intersection((-b + sqrt(d)) / (2 * a), shared_from_this())};
    return out;
  }

  Tuple local_normal_at(const Tuple &p) override {
    auto normal = p - Tuple::point(0, 0, 0);
    //normal.w = 0;
    return normal;
  }

  bool compare(const Shape&) const noexcept override {
    return true;
  }
};

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  if ( !v.empty() ) {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}

std::optional<Intersection> Hit(const std::vector<Intersection> &v) {
  std::vector<Intersection> sorted = v;
  std::sort(sorted.begin(), sorted.end(),
            [](const auto &a, const auto &b) { return a.t() < b.t(); });

  for (const auto &i : sorted) {
    if (i.t() >= 0) {
      return i;
    }
  }
  return {};
}
