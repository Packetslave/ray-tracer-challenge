//
// Created by Brian Landers on 8/8/20.
//

#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include "../core/camera.h"
#include "../core/light.h"
#include "../core/tuple.h"
#include "../shapes/group.h"
#include "../shapes/shape.h"
#include "../shapes/triangle.h"
#include "folly/Conv.h"
#include "folly/String.h"
#include "file.h"

struct FaceVertex {
  size_t v_index;
  size_t t_index;
  size_t n_index;
};

FaceVertex parse_face(const std::string& f) {
  std::vector<std::string> tokens;
  folly::split("/", f, tokens, false);

  size_t vi = folly::to<size_t>(tokens[0]);
  size_t ti = -1;
  size_t ni = -1;

  if (tokens.size() > 1) {
    if (!tokens[1].empty()) {
      ti = folly::to<size_t>(tokens[1]);
    }
    if (!tokens[2].empty()) {
      ni = folly::to<size_t>(tokens[2]);
    }
  }

  return {vi, ti, ni};
}

class ObjFile : public File {
 public:
  explicit ObjFile(const std::string& blob, bool normalize = false)
      : File(blob, normalize),
        ignored_{},
        vertices_{Tuple::point(0, 0, 0)},
        normals_{Tuple::vector(0, 0, 0)},
        faces_{} {
    std::vector<std::string> lines;
    folly::split("\n", blob, lines, true);
    for (const auto& line : lines) {
      if (line.empty()) {
        continue;
      }
      std::vector<std::string> tokens;
      folly::split(" ", line, tokens, true);

      if (tokens[0] == "v") {  // vertex
        vertices_.emplace_back(folly::to<double>(tokens[1]),
                               folly::to<double>(tokens[2]),
                               folly::to<double>(tokens[3]), 1);
        continue;
      }

      if (tokens[0] == "vn") {  // normal
        normals_.emplace_back(folly::to<double>(tokens[1]),
                              folly::to<double>(tokens[2]),
                              folly::to<double>(tokens[3]), 0);
        continue;
      }

      if (tokens[0] == "f") {
        auto parsed1 = parse_face(tokens[1]);
        auto parsed2 = parse_face(tokens[2]);
        auto parsed3 = parse_face(tokens[3]);

        if (tokens.size() == 4) {
          faces_.push_back({parsed1, parsed2, parsed3});
          continue;
        }

        for (size_t i = 2; i < tokens.size() - 1; ++i) {
          auto parsedt1 = parse_face(tokens[1]);
          auto parsedt2 = parse_face(tokens[i]);
          auto parsedt3 = parse_face(tokens[i + 1]);

          faces_.push_back({
              parsedt1,
              parsedt2,
              parsedt3,
          });
        }
        continue;
      }

      std::cout << "Ignored: " << line << std::endl;
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
      if (f[0].n_index == -1) {
        std::shared_ptr<Triangle> t(new Triangle(vertices_[f[0].v_index],
                                                 vertices_[f[1].v_index],
                                                 vertices_[f[2].v_index]));
        default_group_->add(t);
        continue;
      }
      std::shared_ptr<SmoothTriangle> t(
          new SmoothTriangle(vertices_[f[0].v_index], vertices_[f[1].v_index],
                             vertices_[f[2].v_index], normals_[f[0].n_index],
                             normals_[f[1].n_index], normals_[f[2].n_index]));
      default_group_->add(t);
      continue;
    }
    std::cout << "Done parsing: " << vertices_.size() << " points, "
              << normals_.size() << " normals, " << faces_.size() << " faces, "
              << default_group_->children().size()
              << " children in default group." << std::endl;
  }

  Camera* camera() const override { throw std::runtime_error("not implemented"); }
  PointLight* light() const override { throw std::runtime_error("not implemented"); }

  std::shared_ptr<Group> group(const std::string& name) {
    if (named_groups_.find(name) == named_groups_.end()) {
      return nullptr;
    }
    return named_groups_[name];
  }

  uint32_t ignored() const { return ignored_; }

  std::vector<Tuple> vertices() const { return vertices_; }
  std::vector<Tuple> normals() const { return normals_; }

  std::shared_ptr<Group> default_group() { return default_group_; }

 private:
  uint32_t ignored_;
  std::vector<Tuple> vertices_;
  std::vector<Tuple> normals_;
  std::vector<std::array<FaceVertex, 3>> faces_;
};
