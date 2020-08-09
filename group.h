//
// Created by Brian Landers on 8/8/20.
//

#pragma once
#include "shape.h"

class Group : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }

  std::vector<Intersection> local_intersect(const Ray& r) override {
    if (!bounds_of()->intersects(r)) {
      return {};
    }

    std::vector<Intersection> out;

    for (const auto& i : children_) {
      auto result = i->intersects(r);
      for (const auto& j : result) {
        out.push_back(j);
      }
    }

    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
      return a.t() < b.t();
    });

    return out;
  };

  Tuple local_normal_at(const Tuple& p) override {
    return Tuple::vector(0, 0, 0);
  }

  size_t size() const { return children_.size(); }

  template <typename T>
  void add(const std::shared_ptr<T>& s) {
    children_.push_back(s);

    // need to fix parent in shape's children
    if constexpr (std::is_same_v<T, Group>) {
      for (auto& c : s->children()) {
        c->set_parent(s.get());
      }
    }

    s->set_parent(this);
    updated_ = true;
  }

  bool contains(const std::shared_ptr<Shape>& s) {
    return std::find(children_.begin(), children_.end(), s) != children_.end();
  }

  template <typename T>
  std::shared_ptr<T> child(size_t idx) {
    return std::dynamic_pointer_cast<T>(children_[idx]);
  }

  std::vector<std::shared_ptr<Shape>> children() { return children_; }

  BoundingBox* bounds_of() override {
    if (updated_) {
      box_.clear();
      for (const auto& c : children_){
        auto cbox = c->parent_space_bounds();
        box_.add(cbox);
      }
      updated_ = false;
    }
    return &box_;
  }

 private:
  BoundingBox box_;
  bool updated_ = true;
  std::vector<std::shared_ptr<Shape>> children_;
};
