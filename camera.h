#pragma once

#include "canvas.h"
#include "color.h"
#include "matrix.h"
#include "ray.h"
#include "tuple.h"
#include "world.h"

class Camera {
 public:
  Camera(double h, double v, double f)
      : hsize_(h),
        vsize_(v),
        field_of_view_(f),
        pixel_size_(ComputePixelSize(h, v, f)),
        transform_(IDENTITY) {}

  double hsize() { return hsize_; }
  double vsize() { return vsize_; }
  double field_of_view() { return field_of_view_; }

  Matrix* transform() { return &transform_; }
  void set_transform(const Matrix& t) { transform_ = t; }

  double pixel_size() { return pixel_size_; }

  Ray ray_for_pixel(double px, double py) {
    double xoff = (px + 0.5) * pixel_size_;
    double yoff = (py + 0.5) * pixel_size_;

    double world_x = half_width_ - xoff;
    double world_y = half_height_ - yoff;

    auto pixel = transform_.inverse() * Tuple::point(world_x, world_y, -1);
    auto origin = transform_.inverse() * Tuple::point(0.0, 0.0, 0.0);
    auto direction = (pixel - origin).normalize();

    return Ray(origin, direction);
  }

  Canvas render(World w) {
    auto out = Canvas(hsize_, vsize_);

    for (int y = 0; y < vsize_ - 1; ++y) {
      for (int x = 0; x < hsize_ - 1; ++x) {
        auto r = ray_for_pixel(x, y);
        auto c = w.color_at(r);
        out.write_pixel(x, y, c);
      }
    }
    return out;
  }

 private:
  double hsize_;
  double vsize_;
  double field_of_view_;
  double half_width_;
  double half_height_;
  double pixel_size_;
  Matrix transform_;

  double ComputePixelSize(double h, double v, double f) {
    double half_view = tan(f / 2.0);
    double aspect = h / v;

    if (aspect >= 1) {
      half_width_ = half_view;
      half_height_ = half_view / aspect;
    } else {
      half_width_ = half_view * aspect;
      half_height_ = half_view;
    }
    return (half_width_ * 2) / h;
  }
};

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
