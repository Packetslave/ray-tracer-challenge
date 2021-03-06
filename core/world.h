#pragma once

#include <optional>

#include "../shapes/sphere.h"
#include "intersection.h"
#include "light.h"
#include "ray.h"

class World {
 public:
  static World default_world() {
    World w;

    Light* light = new PointLight(Tuple::point(-10, 10, -10), Color(1, 1, 1));
    w.set_light(light);

    auto s1 = new Sphere();

    auto m = Material();
    m.set_color(Color(0.8, 1.0, 0.6));
    m.set_diffuse(0.7);
    m.set_specular(0.2);
    s1->set_material(m);
    w.add(s1);

    auto s2 = new Sphere();
    w.add(s2);

    return w;
  }

  Color shade_hit(const ComputedIntersection& comps, int remaining = 5) {
    auto intensity = light_->intensity_at(comps.over_point, this);

    auto surface = comps.object->material()->lighting(
        comps.object, light_, comps.over_point, comps.eyev, comps.normalv,
        intensity);

    auto reflected = reflected_color(comps, remaining);
    auto refracted = refracted_color(comps, remaining);
    auto material = comps.object->material();

    if (material->reflective() > 0 && material->transparency() > 0) {
      auto ref = comps.schlick();
      return surface + reflected * ref + refracted * (1.0 - ref);
    }
    return surface + reflected + refracted;
  }

  Color color_at(const Ray& r, int remaining = 5) {
    auto intersections = intersect(r);
    auto hit = Hit(intersections);
    if (hit) {
      auto comps = ComputedIntersection(*hit, r);
      return shade_hit(comps, remaining);
    }

    return Color(0, 0, 0);
  }

  Shape* get_object(int index) { return objects_[index]; }

  int size() const { return objects_.size(); }

  void add(Shape* s) { objects_.push_back(s); };

  bool contains(const Shape& s) const {
    for (const auto& i : objects_) {
      if (*i == s) {
        return true;
      }
    }
    return false;
  }

  // TODO: replace with std::optional
  Light* light() const { return light_; }
  void set_light(Light* p) { light_ = p; }

  IntersectionVector intersect(const Ray& r) const {
    IntersectionVector out;
    out.reserve(objects_.size() * 3);

    for (auto& o : objects_) {
      auto hits = o->intersects(r);
      std::move(std::begin(hits), std::end(hits), std::back_inserter(out));
    }

    std::sort(out.begin(), out.end(),
              [](const Intersection& a, const Intersection& b) {
                return a.t() < b.t();
              });
    return out;
  }

  bool is_shadowed(const Tuple& light_position, const Tuple& point) const {
    auto v = light_position - point;
    auto distance = v.magnitude();
    auto direction = v.normalize();

    auto r = Ray(point, direction);
    auto intersections = intersect(r);

    auto h = Hit(intersections);
    return h && h->t() < distance;
  }

  Color reflected_color(const ComputedIntersection& comps, int remaining = 5) {
    if (remaining <= 0) {
      return Color(0, 0, 0);
    }
    if (comps.object->material()->reflective() < EPSILON) {
      return Color(0, 0, 0);
    }
    auto reflect_ray = Ray(comps.over_point, comps.reflectv);
    auto color = color_at(reflect_ray, --remaining);
    return color * comps.object->material()->reflective();
  }

  Color refracted_color(const ComputedIntersection& comps, int remaining = 5) {
    if (comps.object->material()->transparency() == 0) {
      return Color(0, 0, 0);
    }
    if (remaining == 0) {
      return Color(0, 0, 0);
    }
    auto n_ratio = comps.n1 / comps.n2;
    auto cos_i = dot(comps.eyev, comps.normalv);
    auto sin2_t = n_ratio * n_ratio * (1 - cos_i * cos_i);
    if (sin2_t > 1) {
      return Color(0, 0, 0);
    }

    auto cos_t = sqrt(1.0 - sin2_t);
    auto direction =
        comps.normalv * (n_ratio * cos_i - cos_t) - comps.eyev * n_ratio;
    auto refract_ray = Ray(comps.under_point, direction);

    return color_at(refract_ray, remaining - 1) *
           comps.object->material()->transparency();
  }

 private:
  std::vector<Shape*> objects_;
  Light* light_;
};