#include "pattern.h"
#include "shape.h"

Color Pattern::pattern_at_object(const std::shared_ptr<Shape>& obj, Tuple point) const {
  auto obj_point = obj->transform().inverse() * point;
  auto p_point = transform_.inverse() * obj_point;
  return pattern_at(p_point);
}