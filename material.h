#pragma once

#include "color.h"
#include "light.h"

class Material {
 public:
  Material()
      : color_(Color(1, 1, 1)),
        ambient_(0.1),
        diffuse_(0.9),
        specular_(0.9),
        shininess_(200.0) {}

  Color color() const { return color_; }
  double ambient() const { return ambient_; }
  double diffuse() const { return diffuse_; }
  double specular() const { return specular_; }
  double shininess() const { return shininess_; }

  void set_color(const Color &c) { color_ = c; }
  void set_ambient(const double d) { ambient_ = d; }
  void set_diffuse(const double d) { diffuse_ = d; }
  void set_specular(const double d) { specular_ = d; }
  void set_shininess(const double d) { shininess_ = d; }

  Color lighting(PointLight light, Tuple point, Tuple eye_v, Tuple normal_v,
                 bool in_shadow) {
    Color effective = this->color() * light.intensity();
    Tuple light_v = (light.position() - point).normalize();
    Tuple ambient = effective * this->ambient();

    double ldn = dot(light_v, normal_v);

    Color diffuse = Color(0, 0, 0);
    Color specular = Color(0, 0, 0);

    if (ldn >= 0) {
      diffuse = effective * this->diffuse() * ldn;
      Tuple reflect_v = (-light_v).reflect(normal_v);
      double rde = dot(reflect_v, eye_v);

      if (rde > 0) {
        double f = pow(rde, this->shininess());
        specular = light.intensity() * this->specular() * f;
      }
    }
    return in_shadow ? ambient : (ambient + diffuse + specular);
  }

 private:
  Color color_;
  double ambient_;
  double diffuse_;
  double specular_;
  double shininess_;
};

inline bool operator==(const Material &a, const Material &b) {
  return a.color() == b.color() && a.ambient() == b.ambient() &&
         a.diffuse() == b.diffuse() && a.specular() == b.specular() &&
         a.shininess() == b.shininess();
}
