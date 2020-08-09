#pragma once

#include "canvas.h"
#include "color.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/experimental/coro/FutureUtil.h"
#include "folly/experimental/coro/Task.h"
//#include "folly/experimental/coro/Collect.h"
#include "matrix.h"
#include "ray.h"
#include "tuple.h"
#include "world.h"

class Camera {
 public:
  Camera(int h, int v, double f)
      : hsize_(h),
        vsize_(v),
        field_of_view_(f),
        half_width_{0.0},
        half_height_{0.0},
        pixel_size_(ComputePixelSize(h, v, f)),
        transform_(IDENTITY),
        inverse_(IDENTITY) {}

  [[nodiscard]] double hsize() const { return hsize_; }
  [[nodiscard]] double vsize() const { return vsize_; }
  [[nodiscard]] double field_of_view() const { return field_of_view_; }

  Matrix* transform() { return &transform_; }
  void set_transform(const Matrix& t) {
    transform_ = t;
    inverse_ = t.inverse();
  }

  [[nodiscard]] double pixel_size() const { return pixel_size_; }

  Ray ray_for_pixel(double px, double py) {
    double xoff = (px + 0.5) * pixel_size_;
    double yoff = (py + 0.5) * pixel_size_;

    double world_x = half_width_ - xoff;
    double world_y = half_height_ - yoff;

    auto pixel = inverse_ * Tuple::point(world_x, world_y, -1);
    auto origin = inverse_ * Tuple::point(0.0, 0.0, 0.0);
    auto direction = (pixel - origin).normalize();

    return Ray(origin, direction);
  }

  Canvas render(World w) {
    auto out = Canvas(hsize_, vsize_);

    size_t total = vsize_ * hsize_;
    size_t current = 0;

    for (int y = 0; y < vsize_ - 1; ++y) {
      float pct = (float)current / total;
      std::cout << "Now rendering pixel " << current << " of " << total << " ("
                << pct << ")" << std::endl;
      for (int x = 0; x < hsize_ - 1; ++x) {
        auto r = ray_for_pixel(x, y);
        auto c = w.color_at(r);
        out.write_pixel(x, y, c);
        current++;
      }
    }
    return out;
  }

  folly::coro::Task<std::vector<Color>> process_row(World& w, size_t y) {
    if (y % 100 == 0) {
      std::cout << "Executing for row " << y << std::endl;
    }
    std::vector<Color> out;
    for (int x = 0; x < hsize_ - 1; ++x) {
      auto r = ray_for_pixel(x, y);
      auto c = w.color_at(r);
      out.push_back(c);
    }
    co_return out;
  }

  folly::coro::Task<Canvas> multi_render(World w) {
    auto out = Canvas(hsize_, vsize_);
    std::vector<folly::SemiFuture<std::vector<Color>>> futs;
    for (int y = 0; y < vsize_ - 1; ++y) {
      futs.push_back(process_row(w, y).semi());
    }

    std::vector<folly::Try<std::vector<Color>>> results = co_await folly::collectAll(futs.begin(), futs.end());

    for (int y = 0; y < vsize()- 1; ++y) {
      for (int x = 0; x < hsize() - 1; ++x) {
        out.write_pixel(x, y, results[y].value()[x]);
      }
    }
    co_return out;
  }

 private:
  int hsize_;
  int vsize_;
  double field_of_view_;
  double half_width_;
  double half_height_;
  double pixel_size_;
  Matrix transform_;
  Matrix inverse_;

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

Matrix view_transform(const Tuple& from, const Tuple& to, const Tuple& up);