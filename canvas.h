#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "color.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"

inline constexpr std::string_view kPPMHeader = "P3\n%i %i\n%i\n";
inline constexpr std::string_view kPPMElement = "%i %i %i";

namespace {
int clamp(const float x, const float min, const float max) {
  float val = x * 256;
  if (val < min) {
    return min;
  }
  if (val > max) {
    return max;
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
  Color pixel_at(int x, int y) const { return (*pixels_)[index_of(x, y)]; }

  void write_pixel(int x, int y, const Color &c) {
    (*pixels_)[index_of(x, y)] = c;
  }

  int width() const { return width_; };
  int height() const { return height_; };

  std::string to_ppm() const {
    std::vector<std::string> rows;
    std::string header = absl::StrFormat(kPPMHeader, width_, height_, 255);

    std::string row;
    int ctr = 0;
    for (const auto &i : *pixels_) {
      row += absl::StrFormat(kPPMElement, clamp(i.r(), 0, 255),
                             clamp(i.g(), 0, 255), clamp(i.b(), 0, 255));

      if (++ctr == width_) {
        // word wrap algorithm can almost certainly be improved
        if (row.size() > 70) {
          int lastspace = -1;
          unsigned int curpos = 0;
          for (int i = 0; i < row.length(); ++i, ++curpos) {
            if (row[i] == ' ') lastspace = i;
            if (curpos >= 70) {
              if (lastspace > 0) {
                row[lastspace] = '\n';
                curpos = i - lastspace;
                lastspace = -1;
              }
            }
          }
        }
        rows.push_back(std::string(row));

        row.clear();
        ctr = 0;
      } else {
        row += ' ';
      }
    }

    return header + absl::StrJoin(rows, "\n") + '\n';
  }

  void save(const std::string &filename) const {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << this->to_ppm();
    myfile.close();
  }

 private:
  size_t index_of(int x, int y) const { return width_ * y + x; }
  std::unique_ptr<std::vector<Color>> pixels_;
  int width_;
  int height_;
};
