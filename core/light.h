#pragma once

#include "color.h"
#include "tuple.h"

class World;

class Light {
 protected:
  Color intensity_;
  Tuple position_;

 public:
  Light(Tuple position, Color intensity) : intensity_(intensity), position_(position){};
  virtual ~Light() = default;

  Color intensity() { return intensity_; }
  Tuple position() { return position_; }

  virtual bool operator==(const Light& rhs) const {
    return this->intensity_ == rhs.intensity_;
  }

  virtual double intensity_at(const Tuple& point, const World* world) const = 0;
};

class PointLight : public Light {
 protected:

 public:
  PointLight(Tuple position, Color intensity)
      : Light(position, intensity) {}

  double intensity_at(const Tuple& point, const World* world) const override;

  bool operator==(const PointLight& rhs) const {
    return this->intensity_ == rhs.intensity_ &&
           this->position_ == rhs.position_;
  }
};

class AreaLight : public Light {
 protected:
  Tuple corner_;
  Tuple uvec_;
  size_t usteps_;
  Tuple vvec_;
  size_t vsteps_;
  size_t samples_;

 public:
  AreaLight(const Tuple& corner, const Tuple& full_uvec, size_t usteps,
            const Tuple& full_vvec, const size_t vsteps, const Color& intensity)
      : Light(Tuple::point(1, 0, 0.5), intensity),
        corner_(corner),
        usteps_(usteps),
        uvec_(full_uvec / usteps),
        vsteps_(vsteps),
        vvec_(full_vvec / vsteps),
        samples_(usteps * vsteps){}

  Tuple corner() const { return corner_; }
  Tuple uvec() const { return uvec_; }
  size_t usteps() const { return usteps_; }
  Tuple vvec() const { return vvec_; }
  size_t vsteps() const { return vsteps_; }
  size_t samples() const { return samples_; }

  double intensity_at(const Tuple& point, const World* world) const override;

  Tuple point_on(double u, double v) const {
    return corner_ + uvec_ * (u + 0.5) + vvec_ * (v + 0.5);
  }
};