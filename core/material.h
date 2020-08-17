#pragma once

#include "color.h"
#include "light.h"
#include "pattern.h"

class Material {
 public:
  Material()
      : color_(Color(1, 1, 1)),
        ambient_(0.1),
        diffuse_(0.9),
        specular_(0.9),
        shininess_(200.0),
        reflective_(0.0),
        transparency_(0.0),
        refractive_(1.0),
        pattern_(nullptr){}

  Color color() const { return color_; }
  double ambient() const { return ambient_; }
  double diffuse() const { return diffuse_; }
  double specular() const { return specular_; }
  double shininess() const { return shininess_; }
  double reflective() const { return reflective_; }
  double transparency() const { return transparency_; }
  double refractive() const { return refractive_; }
  const Pattern* pattern() const { return pattern_; }

  void set_color(const Color &c) { color_ = c; }
  void set_ambient(const double d) { ambient_ = d; }
  void set_diffuse(const double d) { diffuse_ = d; }
  void set_specular(const double d) { specular_ = d; }
  void set_shininess(const double d) { shininess_ = d; }
  void set_reflective(const double d) { reflective_ = d; }
  void set_transparency(const double d) { transparency_ = d; }
  void set_refractive(const double d) { refractive_ = d; }
  void set_pattern(const Pattern& p) { pattern_ = &p; }

  Color lighting(Shape* obj, Light* light, const Tuple& point, const Tuple& eye_v,
                 const Tuple& normal_v, const double intensity) {
    Color c =
        pattern_ == nullptr ? color_ : pattern_->pattern_at_object(obj, point);

    Color effective = c * light->intensity();
    Tuple ambient = effective * this->ambient();
    auto samples = light->samples();

    Color sum(0, 0, 0);
    for (const auto& sample : samples) {
      Color specular(0, 0, 0);
      Color diffuse(0, 0, 0);

      auto light_v = (sample - point).normalize();
      auto lightDotNormal = dot(light_v, normal_v);

      if (lightDotNormal < 0 or intensity == 0.0) {
        // pass
      } else {
        diffuse = effective * this->diffuse_ * lightDotNormal;

        auto reflect_v = -light_v.reflect(normal_v);
        auto reflectDotEye = dot(reflect_v, eye_v);

        if (reflectDotEye <= 0) {
          // pass
        } else {
          auto factor = pow(reflectDotEye, shininess_);
          specular = light->intensity() * specular_ * factor;
        }
      }
      sum += diffuse;
      sum += specular;
    }
    return ambient + (sum / samples.size()) * intensity;
  }

 private:
  Color color_;
  double ambient_;
  double diffuse_;
  double specular_;
  double shininess_;
  double reflective_;
  double transparency_;
  double refractive_;
  const Pattern* pattern_;
};

inline bool operator==(const Material &a, const Material &b) {
  return a.color() == b.color() && a.ambient() == b.ambient() &&
         a.diffuse() == b.diffuse() && a.specular() == b.specular() &&
         a.shininess() == b.shininess() && a.reflective() == b.reflective() &&
         a.transparency() == b.transparency() &&
         a.refractive() == b.refractive();
}
