#pragma once

#include "canvas.h"
#include "color.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/experimental/coro/FutureUtil.h"
#include "folly/experimental/coro/Task.h"
//#include "folly/experimental/coro/Collect.h"
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_group.h>

#include "../external/oneTBB/include/tbb/blocked_range3d.h"
#include "../external/oneTBB/include/tbb/blocked_range2d.h"
#include "../utils/timer.h"
#include "matrix.h"
#include "ray.h"
#include "tuple.h"
#include "world.h"
#include <tbb/spin_mutex.h>
#include <tbb/parallel_for_each.h>
#include "fmt/format.h"
#include "folly/stop_watch.h"

using Result = std::tuple<size_t, size_t, Color>;

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
  [[nodiscard]] double half_height() const { return half_height_; }
  [[nodiscard]] double half_width() const { return half_width_; }
  [[nodiscard]] Matrix inverse() const { return inverse_; }

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

//  Canvas render(World w) {
//    auto out = Canvas(hsize_, vsize_);
//
//    size_t total = vsize_ * hsize_;
//    size_t current = 0;
//
//    for (int y = 0; y < vsize_ - 1; ++y) {
//      double pct = (double)current / total;
//      std::cout << "Now rendering pixel " << current << " of " << total << " ("
//                << pct << ")" << std::endl;
//      for (int x = 0; x < hsize_ - 1; ++x) {
//        auto r = ray_for_pixel(x, y);
//        auto c = w.color_at(r);
//        out.write_pixel(x, y, std::move(c));
//        current++;
//      }
//    }
//    return out;
//  }

//  struct State {
//    World* world;
//    Camera* camera;
//    Pixel** pixels;
//    int samples = 1;
//    Tuple* origin;
//    double half_h;
//    double half_w;
//  };

//  class ComputePixel {
//    State* state;
//    std::vector<Pixel*>* pixels;
//
//   public:
//    explicit ComputePixel(State* s, std::vector<Pixel*>* pixels) : state{s}, pixels{pixels} {}
//
//    void operator()(const tbb::blocked_range<size_t>& r) const {
//      for (int i = r.begin(); i != r.end(); ++i) {
//        Pixel* pixel = pixels->at(i);
//
//        if (pixel->x == 0 && int(pixel->y) % 100 == 0) {
//          std::cout << "Executing for row " << pixel->y << std::endl;
//        }
//        Color c_out(0, 0, 0);
//
//        // Cheat and look for misses
//        auto actual_r = state->camera->ray_for_pixel(pixel->x, pixel->y);
//        auto hit = state->world->intersect(actual_r);
//        if (hit.empty()) {
//          return;
//        }
//
//        tbb::task_group_context root(tbb::task_group_context::isolated);
//        tbb::parallel_for( 0, 8, 1,
//                      [&]( int  ) {
//        //for (int j = 0; j < state->samples; ++j) {
//                        double xoff = pixel->x + 0.5;
//                        double yoff = pixel->y + 0.5;
//
//                        double rx = drand48();
//                        double ry = drand48();
//
//                        xoff += (0.5 - rx);
//                        yoff += (0.5 - ry);
//
//                        xoff *= state->camera->pixel_size();
//                        yoff *= state->camera->pixel_size();
//
//                        double worldX = state->half_w - xoff;
//                        double worldY = state->half_h - yoff;
//
//                        auto local_pixel = state->camera->inverse_ * Tuple::point(worldX, worldY, -1);
//                        auto direction = (local_pixel - *(state->origin)).normalize();
//
//                        auto r = Ray(*(state->origin), direction);
//                        c_out += state->world->color_at(r);
//            });
//
//        c_out = c_out * (1.0 / state->samples);
//        pixel->c = c_out;
//      }
//    }
//  };

  Canvas multi_render_tbb(World& w, size_t samples) {
    using Timer = folly::stop_watch<std::chrono::seconds>;

    auto origin = inverse_ * Tuple::point(0, 0, 0);
    std::vector<Pixel> pixels(hsize_ * vsize_);

    auto render_t = folly::stop_watch<std::chrono::seconds>();
    std::cout << "START: render" << std::endl;

    //tbb::parallel_for<int>(0, vsize(), 1, [&] (int y) {
    for (size_t y = 0; y < vsize_; ++y) {
      std::unique_ptr<Timer> row_t;
      if (y % 100 == 0) {
        row_t = std::make_unique<Timer>();
        std::cout << "START: row " << y << std::endl;
      }

      // tbb::parallel_for<int>(0, hsize_, 1, [&] (int x) {
      for (size_t x = 0; x < hsize_; ++x) {
        // Timer pixel_t(fmt::format("Rendering pixel {}, {}", x, y));

        // Cheat and look for misses
//        auto actual_r = ray_for_pixel(x, y);
//        auto hit = w.intersect(actual_r);
//        if (hit.empty()) {
//          continue;
//        }

        Color c_out(0, 0, 0);
        // tbb::spin_mutex c_lock;

        if (samples == 1) {
          auto r = ray_for_pixel(x, y);
          pixels[x * y] = Pixel{x, y, w.color_at(r)};
          continue;
        }

        for (size_t sample = 0; sample < samples; ++sample) {
          // Timer sample_t(fmt::format("pixel {}, {} -- sample {}", x, y, sample));

          double xoff = x + 0.5;
          double yoff = y + 0.5;

          double rx = drand48();
          double ry = drand48();

          xoff += (0.5 - rx);
          yoff += (0.5 - ry);

          xoff *= pixel_size();
          yoff *= pixel_size();

          double worldX = half_width_ - xoff;
          double worldY = half_height_ - yoff;

          auto local_pixel = inverse_ * Tuple::point(worldX, worldY, -1);
          auto direction = (local_pixel - origin).normalize();

          auto r = Ray(origin, direction);
          c_out += w.color_at(r);

          // sample_t.stop();
        }  // samples

        c_out = c_out * (1.0 / samples);
        pixels[x * y] = Pixel{x, y, c_out};

        // pixel_t.stop();
      }  // columns

      if (row_t != nullptr) {
        std::cout << "STOP: row " << y << ": " << row_t->elapsed().count() << std::endl;
      }
      //}); // rows
    }
    std::cout << "STOP: render: " << render_t.elapsed().count() << std::endl;

    auto out = Canvas(hsize_, vsize_);
    out.write_pixels(pixels);
    return out;
  }

//  folly::coro::Task<Canvas> multi_render_sampled(World w, size_t samples) {
//    auto origin = inverse_ * Tuple::point(0, 0, 0);
//    auto out = Canvas(hsize_, vsize_);
//    std::vector<folly::SemiFuture<Pixel>> futs;
//    for (int y = 0; y < vsize_ - 1; ++y) {
//      for (int x = 0; x < hsize_ - 1; ++x) {
//        futs.push_back(process_pixel(w, x, y, origin, samples).semi());
//      }
//    }
//    std::vector<folly::Try<Pixel>> results =
//        co_await folly::collectAll(futs.begin(), futs.end());
//
//    for (const auto& p : results) {
//      out.write_pixel(p->x, p->y, p->c);
//    }
//    co_return out;
//  }
//
//  folly::coro::Task<Pixel> process_pixel(World& w, size_t x, size_t y,
//                                         const Tuple& origin, int samples) {
//    if (x == 0 && int(y) % 100 == 0) {
//      std::cout << "Executing for row " << y << std::endl;
//    }
//    Color c_out(0, 0, 0);
//
//    // Cheat and look for misses
//    auto r = ray_for_pixel(x, y);
//    auto hit = w.intersect(r);
//    if (hit.empty()) {
//      co_return {x, y, c_out};
//    }
//
//    for (double i = 0; i < samples; ++i) {
//      double xoff = x + 0.5;
//      double yoff = y + 0.5;
//
//      double rx = drand48();
//      double ry = drand48();
//
//      xoff += (0.5 - rx);
//      yoff += (0.5 - ry);
//
//      xoff *= pixel_size();
//      yoff *= pixel_size();
//
//      double worldX = half_width_ - xoff;
//      double worldY = half_height_ - yoff;
//
//      auto pixel = inverse_ * Tuple::point(worldX, worldY, -1);
//      auto direction = (pixel - origin).normalize();
//
//      auto r = Ray(origin, direction);
//      c_out += w.color_at(r);
//    }
//    c_out = c_out * (1.0 / samples);
//    co_return {x, y, c_out};
//  }
//
//  folly::coro::Task<std::vector<Color>> process_row(World& w, size_t y) {
//    if (y % 100 == 0) {
//      std::cout << "Executing for row " << y << std::endl;
//    }
//    std::vector<Color> out;
//    out.reserve(hsize_);
//
//    for (int x = 0; x < hsize_ - 1; ++x) {
//      out.push_back(w.color_at(ray_for_pixel(x, y)));
//    }
//    co_return out;
//  }
//
//  folly::coro::Task<std::vector<Result>> process_chunk(World& w, size_t x,
//                                                       size_t y,
//                                                       size_t block_size) {
//    std::vector<Result> out;
//    if ((x * y) % 100 == 0) {
//      std::cout << "processing chunk (" << x << ", " << y << ") - block size "
//                << block_size << std::endl;
//    }
//    for (size_t y2 = y; y2 < y + block_size; ++y2) {
//      for (size_t x2 = x; x2 < x + block_size; ++x2) {
//        out.push_back({x2, y2, w.color_at(ray_for_pixel(x2, y2))});
//      }
//    }
//    co_return out;
//  }
//
//  folly::coro::Task<Canvas> multi_render(World w) {
//    auto out = Canvas(hsize_, vsize_);
//    std::vector<folly::SemiFuture<std::vector<Color>>> futs;
//    for (int y = 0; y < vsize_ - 1; ++y) {
//      futs.push_back(process_row(w, y).semi());
//    }
//
//    std::vector<folly::Try<std::vector<Color>>> results =
//        co_await folly::collectAll(futs.begin(), futs.end());
//
//    for (int y = 0; y < vsize() - 1; ++y) {
//      for (int x = 0; x < hsize() - 1; ++x) {
//        out.write_pixel(x, y, results[y].value()[x]);
//      }
//    }
//    co_return out;
//  }
//
//  folly::coro::Task<std::vector<Result>> multi_render2(World w,
//                                                       size_t block_size) {
//    std::vector<folly::SemiFuture<std::vector<Result>>> futs;
//
//    {
//      Timer t("creating futures");
//      size_t i = 0;
//      for (int x = 0; x < hsize_ - 1; x += block_size) {
//        for (int y = 0; y < vsize_ - 1; y += block_size) {
//          futs.push_back(process_chunk(w, x, y, block_size).semi());
//        }
//      }
//    }
//
//    auto result = co_await folly::collectAll(futs.begin(), futs.end());
//
//    std::vector<Result> out;
//    {
//      Timer t("collecting results");
//      for (const auto& rtry : result) {
//        for (const auto& r : rtry.value()) {
//          out.push_back(r);
//        }
//      }
//    }
//    co_return out;
//  }

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
