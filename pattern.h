//
// Created by Brian Landers on 8/7/20.
//

#pragma once
#include "color.h"
#include "matrix.h"

class Shape;

class Pattern {
 public:
  explicit Pattern() : transform_{Matrix(IDENTITY)} {}
  virtual ~Pattern() = default;

  virtual Color pattern_at(const Tuple& point) const = 0;

  void set_transform(Matrix t) { transform_ = t; }
  Matrix transform() const { return transform_; }

  Color pattern_at_object(const std::shared_ptr<Shape>& obj, Tuple point) const;
  Matrix transform_;
};

class TestPattern : public Pattern {
 public:
  Color pattern_at(const Tuple& point) const override {
    return Color(point.x, point.y, point.z);
  }
};

class StripePattern : public Pattern {
 public:
  explicit StripePattern(const Color& a, const Color& b) : Pattern(), a{a}, b{b} {}

  Color pattern_at(const Tuple& point) const override {
    if (int(floor(point.x)) % 2 == 0) {
      return a;
    }
    return b;
  }

  Color a;
  Color b;
};

class GradientPattern : public Pattern {
 public:
  explicit GradientPattern(const Color& a, const Color& b) : Pattern(), a{a}, b{b} {}

  Color pattern_at(const Tuple& point) const override {
    auto distance = b - a;
    auto fraction = point.x - floor(point.x);
    return a + distance * fraction;
  }

  Color a;
  Color b;
};

class RingPattern : public Pattern {
 public:
  explicit RingPattern(const Color& a, const Color& b) : Pattern(), a{a}, b{b} {}

  Color pattern_at(const Tuple& point) const override {
    if (int(floor(sqrt(point.x + point.z))) % 2 == 0) {
      return a;
    }
    return b;
  }

  Color a;
  Color b;
};

class CheckPattern : public Pattern {
 public:
  explicit CheckPattern(const Color& a, const Color& b) : Pattern(), a{a}, b{b} {}

  Color pattern_at(const Tuple& point) const override {
    if (int((abs(point.x) + abs(point.y) + abs(point.z))) % 2 == 0) {
      return a;
    }
    return b;
  }

  Color a;
  Color b;
};