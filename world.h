#pragma once

#include <optional>

#include "intersection.h"
#include "light.h"
#include "ray.h"
#include "sphere.h"

class World {
 public:
  static World default_world() {
    World w;

    PointLight light(Tuple::point(-10, 10, -10), Color(1, 1, 1));
    w.set_light(light);

    std::shared_ptr<Shape> s1;
    s1.reset(new Sphere());

    auto m = Material();
    m.set_color(Color(0.8, 1.0, 0.6));
    m.set_diffuse(0.7);
    m.set_specular(0.2);
    s1->set_material(m);
    w.add(s1);

    std::shared_ptr<Shape> s2;
    s2.reset(new Sphere());

    s2->set_transform(CreateScaling(0.5, 0.5, 0.5));
    w.add(s2);

    return w;
  }

  Color shade_hit(const ComputedIntersection& comps) {
    auto shadowed = is_shadowed(comps.over_point);
    auto out = comps.object->material()->lighting(
        *light_, comps.over_point, comps.eyev, comps.normalv, shadowed);
    return out;
  }

  Color color_at(const Ray& r) {
    auto intersections = intersect(r);
    auto hit = Hit(intersections);
    if (hit) {
      auto comps = ComputedIntersection(*hit, r);
      return shade_hit(comps);
    }

    return Color(0, 0, 0);
  }

  std::shared_ptr<Shape> get_object(int index) {
    return objects_[index];
  }

  int size() const { return objects_.size(); }

  void add(const std::shared_ptr<Shape> s) { objects_.push_back(s); };

  bool contains(const Shape& s) const {
    for (const auto &i: objects_) {
      if (*i == s) {
        return true;
      }
    }
    return false;
  }

  folly::Optional<PointLight> light() { return light_; }
  void set_light(const PointLight& p) { light_ = p; }

  std::vector<Intersection> intersect(const Ray& r) {
    std::vector<Intersection> out;
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

  bool is_shadowed(const Tuple& point) {
    auto v = light_->position() - point;
    auto distance = v.magnitude();
    auto direction = v.normalize();

    auto r = Ray(point, direction);
    auto intersections = intersect(r);

    auto h = Hit(intersections);
    return h && h->t() < distance;
  }

 private:
  std::vector<std::shared_ptr<Shape>> objects_;
  folly::Optional<PointLight> light_;
};