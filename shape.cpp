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
