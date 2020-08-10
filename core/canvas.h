#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "color.h"
#include "fmt/format.h"
#include "folly/String.h"  // TODO: replace with absl

constexpr char const *kPPMHeader = "P3\n{} {}\n{}";
constexpr char const *kPPMElement = "{} {} {}";

namespace {
int clamp(const double x, const double min, const double max) {
  double val = x * 256;
  if (val < min) {
    return (int)min;
  }
  if (val > max) {
    return (int)max;
  }
  return static_cast<int>(val);
}
}  // namespace

class Canvas {
 public:
  Canvas(int width, int height) : width_(width), height_(height) {
    pixels_ =
        std::make_unique<std::vector<Color>>(width * height, Color(0, 0, 0));
  }
  [[nodiscard]] Color pixel_at(int x, int y) const {
    return (*pixels_)[index_of(x, y)];
  }

  void write_pixel(int x, int y, const Color &c) {
    (*pixels_)[index_of(x, y)] = c;
  }

  [[nodiscard]] int width() const { return width_; };
  [[nodiscard]] int height() const { return height_; };

  [[nodiscard]] std::string to_ppm() const {
    std::vector<std::string> rows{
        fmt::format(kPPMHeader, width_, height_, 255)};
    std::string row;
    int ctr = 0;
    for (const auto &i : *pixels_) {
      row += fmt::format(kPPMElement, clamp(i.r(), 0, 255),
                         clamp(i.g(), 0, 255), clamp(i.b(), 0, 255));

      if (++ctr == width_) {
        // word wrap algorithm can almost certainly be improved
        if (row.size() > 70) {
          int lastspace = -1;
          unsigned int curpos = 0;
          for (size_t j = 0; j < row.length(); ++j, ++curpos) {
            if (row[j] == ' ') lastspace = j;
            if (curpos >= 70) {
              if (lastspace > 0) {
                row[lastspace] = '\n';
                curpos = j - lastspace;
                lastspace = -1;
              }
            }
          }
        }
        rows.emplace_back(row);
        row.clear();
        ctr = 0;
      } else {
        row += ' ';
      }
    }
    return folly::join("\n", rows) + '\n';
  }

  void save(const std::string &filename) const {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << this->to_ppm();
    myfile.close();
  }

 private:
  [[nodiscard]] size_t index_of(int x, int y) const { return width_ * y + x; }
  std::unique_ptr<std::vector<Color>> pixels_;
  int width_;
  int height_;
};
