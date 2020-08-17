//
// Created by Brian Landers on 8/8/20.
//

#pragma once
#include "shape.h"

using ShapeVector = std::vector<Shape*>;

class Group : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }

  IntersectionVector local_intersect(const Ray& r) override {
    if (!bounds_of()->intersects(r)) {
      return {};
    }

    IntersectionVector out;
    out.reserve(2 * children_.size());

    for (const auto& i : children_) {
      auto result = i->intersects(r);
      for (const auto& j : result) {
        out.push_back(j);
      }
    }

    std::sort(out.begin(), out.end(),
              [](const auto& a, const auto& b) { return a.t() < b.t(); });

    return out;
  };

  Tuple local_normal_at(const Tuple& p, const Intersection* i) override {
    return Tuple::vector(0, 0, 0);
  }

  size_t size(bool recurse = false) const override {
    if (!recurse) {
      return children_.size();
    }
    size_t count = 0;
    for (const auto& c : children_) {
      count += c->size(recurse);
    }
    return count;
  }

  template <typename T>
  void add(T* s) {
    s->set_parent(this);
    children_.push_back(s);
    updated_ = true;

    //    // need to fix parent in shape's children
    //    if constexpr (std::is_same_v<T, Group>) {
    //      for (auto& c : s->children()) {
    //        c->set_parent(s.get());
    //      }
    //    }
  }

  virtual bool contains(Shape* obj) const override {
    for (const auto& c : children_) {
      if (c->contains(obj)) {
        return false;
      }
    }
    return true;
  }

  template <typename T>
  T* child(size_t idx) {
    return (T*)children_[idx];
  }

  ShapeVector children() { return children_; }

  BoundingBox* bounds_of() override { return bounds_of(true); }

  BoundingBox* bounds_of(bool use_cache) {
    if (updated_ || !use_cache) {
      BoundingBox new_box;
      for (const auto& c : children_) {
        auto cbox = c->parent_space_bounds_of();
        new_box.add(*cbox);
      }
      box_ = new_box;
      updated_ = false;
    }
    return &box_;
  }

  std::pair<ShapeVector, ShapeVector> partition_children() {
    ShapeVector out_left;
    ShapeVector out_right;

    auto bounds = this->bounds_of();
    auto [left, right] = bounds->split();

    ShapeVector::iterator it;
    updated_ = true;

    for (it = children_.begin(); it != children_.end(); /* no increment */) {
      auto child_bounds = (*it)->parent_space_bounds_of();

      if (left.contains(*child_bounds)) {
        out_left.push_back(*it);
        it = children_.erase(it);
        continue;
      }
      if (right.contains(*child_bounds)) {
        out_right.push_back(*it);
        it = children_.erase(it);
        continue;
      }
      ++it;
    }

    return {out_left, out_right};
  }

  void make_subgroup(const ShapeVector& shapes) {
    auto g = new Group();  // TODO: leaks!
    for (const auto& s : shapes) {
      g->add(s);
    }
    add(g);
    updated_ = true;
  }

  void divide(const size_t threshold) override {
    if (threshold <= children_.size()) {
      auto [left, right] = partition_children();
      if (!left.empty()) {
        make_subgroup(left);
      }
      if (!right.empty()) {
        make_subgroup(right);
      }
    }
    for (const auto& i : children_) {
      i->divide(threshold);
    }
    updated_ = true;
  }

 private:
  BoundingBox box_;
  bool updated_ = true;
  ShapeVector children_ = {};
};
