//
// Created by Brian Landers on 7/27/20.
//

#include "shape.h"

bool Shape::operator==(const Shape &other) const noexcept {
  return typeid(*this) == typeid(other) && transform_ == other.transform_ &&
         material_ == other.material_ && compare(other);
}

bool Shape::operator!=(const Shape &other) const noexcept {
  return !(*this == other);
}

Tuple Shape::worldToObject(const Tuple &point) {
  Tuple p = point;
  if (parent_ != nullptr) {
    p = parent_->worldToObject(p);
  }
  return this->transform().inverse() * p;
};

std::optional<Intersection> Hit(const IntersectionVector &v) {
  IntersectionVector sorted = v;
  std::sort(sorted.begin(), sorted.end(),
            [](const auto &a, const auto &b) { return a.t() < b.t(); });

  for (const auto &i : sorted) {
    if (i.t() >= 0) {
      return i;
    }
  }
  return {};
}
