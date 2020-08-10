//
// Created by Brian Landers on 7/26/20.
//

#include "camera.h"

Matrix view_transform(const Tuple& from, const Tuple& to, const Tuple& up) {
  auto forward = (to - from).normalize();
  auto left = cross(forward, up.normalize());
  auto true_up = cross(left, forward);

  // clang-format off
  auto out = Matrix{MatrixData4{
      {
          { left.x,     left.y,     left.z,    0.0 },
          { true_up.x,  true_up.y,  true_up.z, 0.0 },
          {-forward.x, -forward.y, -forward.z, 0.0 },
          { 0.0,        0.0,        0.0,       1.0 }
      }
  }};
  // clang-format on
  return out * CreateTranslation(-from.x, -from.y, -from.z);
}
