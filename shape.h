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
        material_(Material()),
        parent_{nullptr} {}

  virtual ~Shape() = default;

  virtual bool operator==(const Shape &) const noexcept;
  virtual bool operator!=(const Shape &) const noexcept;

  virtual bool compare(const Shape &) const noexcept = 0;

  Matrix transform() { return transform_; }
  Matrix inverse() { return inverse_; }

  Shape* parent() { return parent_; }
  void set_parent(Shape* p) { parent_ = p; }

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
  Tuple worldToObject(const Tuple &point);
  Tuple normalToWorld(const Tuple &normalVector) {
    Tuple world_normal = this->inverse_.transpose() * normalVector;
    world_normal.w = 0;
    world_normal = world_normal.normalize();

    if (parent_ != nullptr) {
      world_normal = parent_->normalToWorld(world_normal);
    }
    return world_normal;
  };

 protected:
  Matrix transform_;
  Matrix inverse_;
  Material material_;
  Shape* parent_;

 private:
  Tuple objectToWorld(const Tuple &point) { return this->transform_ * point; };

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
  double schlick() const {
    auto cos = dot(eyev, normalv);

    if (n1 > n2) {
      auto n = n1 / n2;
      auto sin2_t = n * n * (1.0 - cos * cos);

      if (sin2_t > 1.0) {
        return 1.0;
      }

      auto cos_t = sqrt(1.0 - sin2_t);
      cos = cos_t;
    }

    auto r0 = pow(((n1 - n2) / (n1 + n2)), 2);
    return r0 + (1 - r0) * pow(1 - cos, 5);
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

class Group : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }

  std::vector<Intersection> local_intersect(const Ray& r) override {
    std::vector<Intersection> out;

    for (const auto& i : children_) {
      auto result = i->intersects(r);
      for (const auto& j : result) {
        out.push_back(j);
      }
    }

    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
      return a.t() < b.t();
    });

    return out;
  };

  Tuple local_normal_at(const Tuple& p) override {
    return Tuple::vector(0, 0, 0);
  }

  size_t size() const { return children_.size(); }

  template <typename T>
  void add(const std::shared_ptr<T>& s) {
    children_.push_back(s);

    // need to fix parent in shape's children
    if constexpr (std::is_same_v<T, Group>) {
      for (auto& c : s->children()) {
        c->set_parent(s.get());
      }
    }

    s->set_parent(this);
  }

  bool contains(const std::shared_ptr<Shape>& s) {
    return std::find(children_.begin(), children_.end(), s) != children_.end();
  }

  template <typename T>
  std::shared_ptr<T> child(size_t idx) {
    return std::dynamic_pointer_cast<T>(children_[idx]);
  }

  std::vector<std::shared_ptr<Shape>> children() { return children_; }

 private:
  std::vector<std::shared_ptr<Shape>> children_;
};