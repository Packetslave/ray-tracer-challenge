//
// Created by Brian Landers on 7/27/20.
//

#pragma once

#include "intersection.h"
#include "material.h"
#include "matrix.h"
#include "ray.h"

#define WARN(msg)                                          \
  fprintf(stderr, "warning: %s:%d: ", __FILE__, __LINE__); \
  fprintf(stderr, "%s\n", msg);

class Shape : public std::enable_shared_from_this<Shape> {
 public:
  explicit Shape()
      : transform_(Matrix(IDENTITY)),
        inverse_(Matrix(IDENTITY)),
        material_(Material()) {}

  virtual ~Shape() = default;

  virtual bool operator==(const Shape &) const noexcept;
  virtual bool operator!=(const Shape &) const noexcept;

  virtual bool compare(const Shape &) const noexcept = 0;

  Matrix transform() { return transform_; }
  Matrix inverse() { return inverse_; }

  void set_transform(const Matrix &t) {
    transform_ = t;
    inverse_ = transform_.inverse();
  }

  Material *material() { return &material_; }

  void set_material(const Material &m) { material_ = m; }

  std::vector<Intersection> intersects(const Ray &r) {
    auto local_ray = r.transform(inverse_);
    return local_intersect(local_ray);
  };

  Tuple normal_at(const Tuple &p) {
    auto local_point = worldToObject(p);
    auto local_normal = local_normal_at(local_point);
    auto world_normal = normalToWorld(local_normal);
    return world_normal;
  }

  virtual std::vector<Intersection> local_intersect(const Ray &r) = 0;
  virtual Tuple local_normal_at(const Tuple &p) = 0;

 protected:
  Matrix transform_;
  Matrix inverse_;
  Material material_;

 private:
  Tuple worldToObject(const Tuple &point) { return this->inverse_ * point; };
  Tuple objectToWorld(const Tuple &point) { return this->transform_ * point; };
  Tuple normalToWorld(const Tuple &normalVector) {
    Tuple world_normal = this->inverse_.transpose() * normalVector;
    world_normal.w = 0;
    return world_normal.normalize();
  };
};

struct ComputedIntersection {
  ComputedIntersection(Intersection hit, Ray r,
                       std::vector<Intersection> xs = {})
      : object(hit.object()),
        t(hit.t()),
        point(r.position(t)),
        eyev(-r.direction()),
        normalv(object->normal_at(point)),
        over_point(Tuple::point(0, 0, 0)),
        under_point(Tuple::point(0, 0, 0)),
        reflectv(Tuple::vector(0, 0, 0)),
        inside(false),
        n1{0.0},
        n2{0.0} {
    if (dot(normalv, eyev) < 0) {
      inside = true;
      normalv = -normalv;
    }
    reflectv = r.direction().reflect(normalv);
    over_point = point + normalv * EPSILON;
    under_point = point - normalv * EPSILON;

    std::vector<std::shared_ptr<Shape>> containers;
    for (const auto &i : xs) {
      if (i == hit) {
        if (containers.empty()) {
          this->n1 = 1.0;
        } else {
          this->n1 =
              containers[containers.size() - 1]->material()->refractive();
        }
      }
      auto it =
          std::find_if(std::begin(containers), std::end(containers),
                       [&i](const auto &c) { return *c.get() == *i.object(); });
      if (it != containers.end()) {
        containers.erase(it);
      } else {
        containers.push_back(i.object());
      }

      if (i == hit) {
        if (containers.empty()) {
          this->n2 = 1.0;
        } else {
          this->n2 =
              containers[containers.size() - 1]->material()->refractive();
        }
        break;
      }
    }
  }

  std::shared_ptr<Shape> object;
  double t;
  Tuple point;
  Tuple eyev;
  Tuple normalv;
  Tuple over_point;
  Tuple under_point;
  Tuple reflectv;
  bool inside;
  double n1;
  double n2;
};
