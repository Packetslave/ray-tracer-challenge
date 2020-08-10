//
// Created by Brian Landers on 8/8/20.
//

#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include "../core/tuple.h"
#include "../shapes/group.h"
#include "../shapes/shape.h"
#include "../shapes/triangle.h"
#include "folly/Conv.h"
#include "folly/String.h"

size_t parse_face(const std::string& f) {
  size_t slash = f.find('/');
  if (slash == -1) {
    return folly::to<size_t>(f);
  }
  return folly::to<size_t>(f.substr(0, slash));
}

class ObjFile {
 public:
  explicit ObjFile(const std::string& blob, bool normalize = false)
      : ignored_{},
        vertices_{Tuple::point(0, 0, 0)},
        faces_{},
        default_group_(std::make_shared<Group>()),
        named_groups_({}) {
    std::vector<std::string> lines;
    folly::split("\n", blob, lines, true);
    for (const auto& line : lines) {
      if (line.empty()) {
        continue;
      }
      std::vector<std::string> tokens;
      folly::split(" ", line, tokens, true);

      if (tokens[0] == "v") {
        vertices_.emplace_back(folly::to<double>(tokens[1]),
                               folly::to<double>(tokens[2]),
                               folly::to<double>(tokens[3]), 1);
        continue;
      }

      if (tokens[0] == "f") {
        if (tokens.size() == 4) {
          faces_.push_back({
              parse_face(tokens[1]),
              parse_face(tokens[2]),
              parse_face(tokens[3]),
          });
          continue;
        }

        for (size_t i = 2; i < tokens.size() - 1; ++i) {
          faces_.push_back({
              parse_face(tokens[1]),
              parse_face(tokens[i]),
              parse_face(tokens[i + 1]),
          });
        }
        continue;
      }
      //
      //      if (tokens[0] == "g") {
      //        named_groups_[tokens[1]] = std::make_shared<Group>();
      //        current_group_ = tokens[1];
      //        continue;
      //      }

      ignored_++;
    }  // lines

    if (normalize) {
      double min_x, min_y, min_z;
      double max_x, max_y, max_z;

      for (const auto& v : vertices_) {
        if (v.x > max_x) {
          max_x = v.x;
        }
        if (v.y > max_y) {
          max_y = v.y;
        }
        if (v.z > max_z) {
          max_z = v.z;
        }
        if (v.x < min_x) {
          min_x = v.x;
        }
        if (v.y < min_y) {
          min_y = v.y;
        }
        if (v.z < min_z) {
          min_z = v.z;
        }
      }

      auto sx = max_x - min_x;
      auto sy = max_y - min_y;
      auto sz = max_z - min_z;

      auto scale = std::max({sx, sy, sz}) / 2;

      for (auto& v : vertices_) {
        v.x = (v.x - (min_x + sx / 2)) / scale;
        v.y = (v.y - (min_y + sy / 2)) / scale;
        v.z = (v.z - (min_z + sz / 2)) / scale;
      }
    }

    for (const auto& f : faces_) {
      std::shared_ptr<Triangle> t(
          new Triangle(vertices_[f[0]], vertices_[f[1]], vertices_[f[2]]));
      default_group_->add(t);
    }
  }

  std::shared_ptr<Group> group(const std::string& name) {
    if (named_groups_.find(name) == named_groups_.end()) {
      return nullptr;
    }
    return named_groups_[name];
  }

  uint32_t ignored() const { return ignored_; }

  std::vector<Tuple> vertices() const { return vertices_; }

  std::shared_ptr<Group> default_group() { return default_group_; }

  std::shared_ptr<Group> to_group() {
    auto group = std::make_shared<Group>();
    for (auto& c : default_group_->children()) {
      c->set_parent(group.get());
      group->add(c);
    }

    for (const auto& [k, v] : named_groups_) {
      group->add<Group>(v);
    }
    return group;
  }

 private:
  uint32_t ignored_;
  std::vector<Tuple> vertices_;
  std::vector<std::vector<size_t>> faces_;
  std::unordered_map<std::string, std::shared_ptr<Group>> named_groups_;
  std::shared_ptr<Group> default_group_;
  std::string current_group_;
};
