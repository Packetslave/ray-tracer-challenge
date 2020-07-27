#pragma once

#include <optional>
#include <vector>

#include "intersection.h"
#include "material.h"
#include "ray.h"

class Sphere {
 public:
  explicit Sphere() : transform_(Matrix{IDENTITY}), material_(Material()) {}

  // FIXME: this can be optimized a fair bit
  std::vector<Intersection> intersects(const Ray &r) {
    std::vector<Intersection> out;
    Matrix t = transform_.inverse(false);
    auto r2 = r.transform(t);

    auto sphere_to_ray = r2.origin() - Tuple::point(0, 0, 0);
    auto a = dot(r2.direction(), r2.direction());
    auto b = 2 * dot(r2.direction(), sphere_to_ray);
    auto c = dot(sphere_to_ray, sphere_to_ray) - 1;
    auto d = (b * b) - (4 * a * c);

    if (d < 0) {
      return out;
    }

    out.emplace_back(Intersection((-b - sqrt(d)) / (2 * a), this));
    out.emplace_back(Intersection((-b + sqrt(d)) / (2 * a), this));
    return out;
  }

  Tuple normal_at(const Tuple &p) {
    auto object_point = transform_.inverse(false) * p;
    auto object_normal = object_point - Tuple::point(0, 0, 0);
    auto world_normal = transform_.inverse(false).transpose() * object_normal;
    world_normal.w = 0;
    return world_normal.normalize();
  }

  Matrix transform() { return transform_; }

  void set_transform(const Matrix &t) { transform_ = t; }

  Material *material() { return &material_; }

  void set_material(const Material &m) { material_ = m; }

  bool operator==(const Sphere &rhs) const {
    return this->transform_ == rhs.transform_ &&
           this->material_ == rhs.material_;
  }

 private:
  Matrix transform_;
  Material material_;
};

struct ComputedIntersection {
  ComputedIntersection(Intersection i, Ray r)
      : object(i.object()),
        t(i.t()),
        point(r.position(t)),
        eyev(-r.direction()),
        normalv(object->normal_at(point)),
        over_point(point + normalv * EPSILON)
         {
    if (dot(normalv, eyev) < 0) {
      inside = true;
      normalv = -normalv;
    }
  }

  Sphere *object;
  double t;
  Tuple point;
  Tuple eyev;
  Tuple normalv;
  Tuple over_point;
  bool inside = false;
};

std::optional<Intersection> Hit(const std::vector<Intersection> &v) {
  std::vector<Intersection> sorted = v;
  std::sort(sorted.begin(), sorted.end(),
            [](auto a, auto b) { return a.t() < b.t(); });
  for (const auto &i : sorted) {
    if (i.t() >= 0) {
      return i;
    }
  }
  return {};
}
