//
// Created by Brian Landers on 8/8/20.
//

#pragma once

#include "matrix.h"
#include "ray.h"
#include "tuple.h"

class BoundingBox {
 public:
  explicit BoundingBox() : min_{POSITIVE_INF}, max_{NEGATIVE_INF} {}
  BoundingBox(const Tuple& min, const Tuple& max) : min_{min}, max_{max} {}

  bool empty() { return true; }
  Tuple min() const { return min_; }
  Tuple max() const { return max_; }

  void add(const Tuple& p) {
    if (p.x < min_.x) {
      min_.x = p.x;
    }
    if (p.y < min_.y) {
      min_.y = p.y;
    }
    if (p.z < min_.z) {
      min_.z = p.z;
    }

    if (p.x > max_.x) {
      max_.x = p.x;
    }
    if (p.y > max_.y) {
      max_.y = p.y;
    }
    if (p.z > max_.z) {
      max_.z = p.z;
    }
  }

  bool intersects(const Ray& r) {
    auto [ytmin, ytmax] = check_axis(r.origin().y, r.direction().y, min_.y, max_.y);
    auto [ztmin, ztmax] = check_axis(r.origin().z, r.direction().z, min_.z, max_.z);
    auto [xtmin, xtmax] = check_axis(r.origin().x, r.direction().x, min_.x, max_.x);

    auto tmin = std::max({xtmin, ytmin, ztmin});
    auto tmax = std::min({xtmax, ytmax, ztmax});

    if (tmin > tmax) {
      return false;
    }
    return true;

  }

  void add(const BoundingBox& b2) {
    add(b2.min());
    add(b2.max());
  }

  bool contains(const Tuple& p) {
    return (p.x >= min_.x && p.x <= max_.x) and
           (p.y >= min_.y && p.y <= max_.y) and
           (p.z >= min_.z && p.z <= max_.z);
  }

  bool contains(const BoundingBox& b2) {
    return contains(b2.min()) && contains(b2.max());
  }

  BoundingBox transform(const Matrix& m) {
    auto p1 = min_;
    auto p2 = Tuple::point(min_.x, min_.y, max_.z);
    auto p3 = Tuple::point(min_.x, max_.y, min_.z);
    auto p4 = Tuple::point(min_.x, max_.y, max_.z);
    auto p5 = Tuple::point(max_.x, min_.y, min_.z);
    auto p6 = Tuple::point(max_.x, min_.y, max_.z);
    auto p7 = Tuple::point(max_.x, max_.y, min_.z);
    auto p8 = max_;

    auto new_bbox = BoundingBox();

    for(const auto& p : {p1, p2, p3, p4, p5, p6, p7, p8}) {
      new_bbox.add(m * p);
    }
    return new_bbox;
  }

  std::pair<double, double> check_axis(const double origin, const double direction, const double min, const double max) {
    auto tmin_num = (min - origin);
    auto tmax_num = (max - origin);

    double tmin, tmax;
    if (abs(direction) >= EPSILON) {
      tmin = tmin_num / direction;
      tmax = tmax_num / direction;
    } else {
      tmin = tmin_num * INFINITY;
      tmax = tmax_num * INFINITY;
    }
    if (tmin > tmax) {
      auto tmp = tmin;
      tmin = tmax;
      tmax = tmp;
    }
    return {tmin, tmax};
  }

  void clear() {
    min_ = POSITIVE_INF;
    max_ = NEGATIVE_INF;
  }

  std::pair<BoundingBox, BoundingBox> split() {
    auto dx = abs(min_.x - max_.x);
    auto dy = abs(min_.y - max_.y);
    auto dz = abs(min_.z - max_.z);

    auto greatest = std::max({dx, dy, dz});

    auto x0 = min_.x;
    auto y0 = min_.y;
    auto z0 = min_.z;
    auto x1 = max_.x;
    auto y1 = max_.y;
    auto z1 = max_.z;

    if (greatest == dx) {
      x1 = x0 + dx / 2.0;
      x0 = x1;
    } else if (greatest == dy) {
      y1 = y0 + dy / 2.0;
      y0 = y1;
    } else {
      z1 = z0 + dz / 2.0;
      z0 = z1;
    }

    auto mid_min = Tuple::point(x0, y0, z0);
    auto mid_max = Tuple::point(x1, y1, z1);

    auto left = BoundingBox(min_, mid_max);
    auto right = BoundingBox(mid_min, max_);

    return { left, right };
  }

 private:
  Tuple min_;
  Tuple max_;
};
