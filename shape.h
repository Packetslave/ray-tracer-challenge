//
// Created by Brian Landers on 7/27/20.
//

#pragma once

#include "intersection.h"
#include "material.h"
#include "matrix.h"
#include "ray.h"

#define WARN(msg) \
    fprintf(stderr, "warning: %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, "%s\n", msg);

class Shape : public std::enable_shared_from_this<Shape> {
 public:
  explicit Shape()
      : transform_(Matrix(IDENTITY)),
        material_(Material()) {}

  virtual ~Shape() = default;

  virtual bool operator==(const Shape&) const noexcept;
  virtual bool operator!=(const Shape&) const noexcept;

  virtual bool compare(const Shape&) const noexcept = 0;

  Matrix transform() { return transform_; }

  void set_transform(const Matrix &t) { transform_ = t; }

  Material *material() { return &material_; }

  void set_material(const Material &m) { material_ = m; }

  std::vector<Intersection> intersects(const Ray &r) {
    auto local_ray = r.transform(transform_.inverse());
    return local_intersect(local_ray);
  };

  Tuple normal_at(const Tuple &p) {
    auto local_point = worldToObject(p);
    auto local_normal = local_normal_at(local_point);
    auto world_normal = normalToWorld(local_normal);
    return world_normal;
  }

  virtual std::vector<Intersection> local_intersect(const Ray &r) = 0;
  virtual Tuple local_normal_at(const Tuple& p)  = 0;

 protected:
  Matrix transform_;
  Material material_;

 private:
  Tuple worldToObject(const Tuple& point) { return this->transform_.inverse() * point; };
  Tuple objectToWorld(const Tuple& point) { return this->transform_ * point; };
  Tuple normalToWorld(const Tuple& normalVector) {
    Tuple world_normal = this->transform_.inverse().transpose() * normalVector;
    world_normal.w = 0;
    return world_normal.normalize();
  };
};

struct ComputedIntersection {
  ComputedIntersection(Intersection i, Ray r)
      : object(i.object()),
        t(i.t()),
        point(r.position(t)),
        eyev(-r.direction()),
        normalv(object->normal_at(point)),
        over_point(Tuple::point(0, 0, 0)),
        reflectv(Tuple::vector(0, 0, 0)),
        inside(false){
    if (dot(normalv, eyev) < 0) {
      inside = true;
      normalv = -normalv;
    }
    reflectv = r.direction().reflect(normalv);
    over_point = point + normalv * EPSILON;
  }

  std::shared_ptr<Shape> object;
  double t;
  Tuple point;
  Tuple eyev;
  Tuple normalv;
  Tuple over_point;
  Tuple reflectv;
  bool inside;
};
