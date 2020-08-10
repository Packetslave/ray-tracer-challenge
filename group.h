//
// Created by Brian Landers on 8/8/20.
//

#pragma once
#include "shape.h"


using ShapeVector = std::vector<std::shared_ptr<Shape>>;

class Group : public Shape {
 public:
  bool compare(const Shape&) const noexcept override { return true; }

  IntersectionVector local_intersect(const Ray& r) override {
    if (!bounds_of()->intersects(r)) {
      return {};
    }

    IntersectionVector out;

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
    return bounds_of(true);
  }

  BoundingBox* bounds_of(bool use_cache)  {
    if (!use_cache || updated_) {
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

    std::vector<std::shared_ptr<Shape>>::iterator it;

    for (it = children_.begin(); it != children_.end(); /* no increment */) {
      auto child_bounds = (*it)->parent_space_bounds_of();

      if (left.contains(*child_bounds)) {
        out_left.push_back(*it);
        it = children_.erase(it);
        updated_ = true;
        continue;
      }
      if (right.contains(*child_bounds)) {
        out_right.push_back(*it);
        it = children_.erase(it);
        updated_ = true;
        continue;
      }
      ++it;
    }

    return { out_left, out_right };
  }

  void make_subgroup(ShapeVector shapes) {
    auto g = std::make_shared<Group>();
    for (const auto& s : shapes) {
      g->add(s);
    }
    add(g);
  }

  void divide(const size_t threshold) override {
    if (threshold <= children_.size()) {
      auto [left, right] = partition_children();
      if (! left.empty()) {
        make_subgroup(left);
      }
      if (! right.empty()) {
        make_subgroup(right);
      }
    }
//    for (const auto& i : children_) {
//      i->divide(threshold);
//    }
  }

 private:
  BoundingBox box_;
  bool updated_ = true;
  std::vector<std::shared_ptr<Shape>> children_;
};
