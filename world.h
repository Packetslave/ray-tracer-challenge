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

    auto s1 = Sphere();

    auto m = Material();
    m.set_color(Color(0.8, 1.0, 0.6));
    m.set_diffuse(0.7);
    m.set_specular(0.2);
    s1.set_material(m);
    w.add(s1);

    auto s2 = Sphere();
    s2.set_transform(CreateScaling(0.5, 0.5, 0.5));
    w.add(s2);

    return w;
  }

  Color shade_hit(const ComputedIntersection& comps) {
    auto out = comps.object->material()->lighting(*light_, comps.point,
                                                  comps.eyev, comps.normalv);
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

  Sphere* get_object(int index) { return &(objects_[index]); }

  int size() const { return objects_.size(); }

  void add(const Sphere& s) { objects_.push_back(s); }

  bool contains(const Sphere& s) const {
    return std::find(objects_.begin(), objects_.end(), s) != objects_.end();
  }

  std::optional<PointLight> light() { return light_; }
  void set_light(const PointLight& p) { light_ = p; }

  std::vector<Intersection> intersect(const Ray& r) {
    std::vector<Intersection> out;

    for (auto& o : objects_) {
      auto hits = o.intersects(r);
      for (const auto& h : hits) {
        out.push_back(h);
      }
    }

    std::sort(out.begin(), out.end(),
              [](const Intersection& a, const Intersection& b) {
                return a.t() < b.t();
              });
    return out;
  }

 private:
  std::vector<Sphere> objects_;
  std::optional<PointLight> light_;
};