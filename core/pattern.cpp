#include "pattern.h"

#include "../shapes/shape.h"

Color Pattern::pattern_at_object(Shape* obj, Tuple point) const {
  auto obj_point = obj->transform().inverse() * point;
  auto p_point = transform_.inverse() * obj_point;
  return pattern_at(p_point);
}
