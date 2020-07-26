//
// Created by Brian Landers on 2018-12-14.
//

#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

#include "tuple.h"

constexpr double PI = 3.14159265358979323846;
constexpr double PI_2 = 1.57079632679489661923;
constexpr double PI_4 = 0.785398163397448309616;

using MatrixData4 = std::array<std::array<double, 4>, 4>;
using MatrixData3 = std::array<std::array<double, 3>, 3>;
using MatrixData2 = std::array<std::array<double, 2>, 2>;

namespace std {
template <>
struct hash<MatrixData4> {
  std::size_t operator()(const MatrixData4 &m) const noexcept {
    size_t seed = 0;
    for (size_t j = 0; j < 4; ++j) {
      for (size_t i = 0; i < 4; ++i) {
        seed ^= std::hash<double>()(m[i][j]) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
      }
    }
    return seed;
  }
};
}  // namespace std

using MatrixCache = std::unordered_map<MatrixData4, MatrixData4>;

constexpr MatrixData4 IDENTITY = MatrixData4{{
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0},
}};

namespace {
MatrixCache makeCache() { return MatrixCache(); }
}  // namespace

class Matrix;
float determinant(const MatrixData2 &m);
float determinant(const MatrixData3 &m);
float determinant(const MatrixData4 &m);
float cofactor(const MatrixData4 &m, size_t row, size_t column);
float cofactor(const MatrixData3 &m, size_t row, size_t column);
float cofactor(const MatrixData2 &m, size_t row, size_t column);
MatrixData3 submatrix(const MatrixData4 &m, size_t row, size_t column);
MatrixData2 submatrix(const MatrixData3 &m, size_t row, size_t column);
MatrixData4 get_inverse(const MatrixData4 &m);

class Matrix {
 public:
  static const Matrix cache_lookup(const MatrixData4 &key) {
    static MatrixCache cache = makeCache();

    const auto it = cache.find(key);
    if (it != cache.end()) {
      return Matrix(it->second);
    }
    const auto inserted = cache.emplace(key, get_inverse(key));
    return Matrix(inserted.first->second);
  }

  explicit Matrix() {
    for (size_t i = 0; i < 4; ++i) {
      for (size_t j = 0; j < 4; ++j) {
        data_[i][j] = 0.0;
      }
    }
  }

  explicit Matrix(const MatrixData4 &values) : Matrix() {
    for (size_t i = 0; i < 4; ++i) {
      for (size_t j = 0; j < 4; ++j) {
        data_[i][j] = values[i][j];
      }
    }
  }

  // For testing only
  const MatrixData4 *TEST_get_data() const { return &data_; }

  void print() {
    for (size_t i = 0; i < 4; ++i) {
      for (size_t j = 0; j < 4; ++j) {
        std::cout << data_[i][j] << "\t";
      }
      std::cout << std::endl;
    }
  }

  float get(const size_t row, const size_t column) const {
    assert(row < 4);
    assert(column < 4);
    return data_[row][column];
  }

  void set(const size_t row, const size_t column, const float value) {
    assert(row < 4);
    assert(column < 4);
    data_[row][column] = value;
  }

  size_t size() const { return 4; }

  Matrix transpose() const {
    Matrix out;
    for (size_t i = 0; i < 4; ++i) {
      for (size_t j = 0; j < 4; ++j) {
        out.set(j, i, data_[i][j]);
      }
    }
    return out;
  }

  bool is_invertible() const { return determinant(data_) != 0; }

  Matrix inverse(bool use_cache = true) { return cache_lookup(data_); }

 private:
  MatrixData4 data_ = MatrixData4{{
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
  }};
};

inline bool operator==(const Matrix &a, const Matrix &b) {
  size_t size = a.size();
  if (b.size() != size) {
    return false;
  }

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (a.get(i, j) != b.get(i, j)) {
        return false;
      }
    }
  }
  return true;
}

inline bool operator!=(const Matrix &a, const Matrix &b) { return !(a == b); }

inline Matrix operator*(const Matrix &a, const Matrix &b) {
  Matrix out;

  for (size_t row = 0; row < 4; ++row) {
    for (size_t col = 0; col < 4; ++col) {
      float sum = 0.0;
      for (size_t i = 0; i < 4; ++i) {
        sum += (a.get(row, i) * b.get(i, col));
      }
      out.set(row, col, sum);
    }
  }
  return out;
}

inline Tuple operator*(const Matrix &a, const Tuple &b) {
  float out[4];
  for (size_t i = 0; i < 4; ++i) {
    out[i] = a.get(i, 0) * b.x + a.get(i, 1) * b.y + a.get(i, 2) * b.z +
             a.get(i, 3) * b.w;
  }
  return {
      out[0],
      out[1],
      out[2],
      out[3],
  };
}

Matrix CreateTranslation(const float x, const float y, const float z) {
  Matrix out{IDENTITY};
  out.set(0, 3, x);
  out.set(1, 3, y);
  out.set(2, 3, z);
  return out;
}

Matrix CreateScaling(const float x, const float y, const float z) {
  Matrix out{IDENTITY};
  out.set(0, 0, x);
  out.set(1, 1, y);
  out.set(2, 2, z);
  return out;
}

Matrix CreateRotationX(const float radians) {
  Matrix out{IDENTITY};
  out.set(1, 1, cos(radians));
  out.set(1, 2, -sin(radians));
  out.set(2, 1, sin(radians));
  out.set(2, 2, cos(radians));
  return out;
}

Matrix CreateRotationY(const float radians) {
  Matrix out{IDENTITY};
  out.set(0, 0, cos(radians));
  out.set(0, 2, sin(radians));
  out.set(2, 0, -sin(radians));
  out.set(2, 2, cos(radians));
  return out;
}

Matrix CreateRotationZ(const float radians) {
  Matrix out{IDENTITY};
  out.set(0, 0, cos(radians));
  out.set(0, 1, -sin(radians));
  out.set(1, 0, sin(radians));
  out.set(1, 1, cos(radians));
  return out;
}

Matrix CreateShearing(const float xy, const float xz, const float yx,
                      const float yz, const float zx, const float zy) {
  Matrix out{IDENTITY};
  out.set(0, 1, xy);
  out.set(0, 2, xz);
  out.set(1, 0, yx);
  out.set(1, 2, yz);
  out.set(2, 0, zx);
  out.set(2, 1, zy);
  return out;
}

float determinant(const MatrixData2 &m) {
  return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

float determinant(const MatrixData3 &m) {
  float out = 0.0;
  for (size_t i = 0; i < 3; ++i) {
    out += m[0][i] * cofactor(m, 0, i);
  }
  return out;
}

float determinant(const MatrixData4 &m) {
  float out = 0.0;
  for (size_t i = 0; i < 4; ++i) {
    out += (m[0][i] * cofactor(m, 0, i));
  }
  return out;
}

float minor(const MatrixData3 &m, size_t row, size_t column) {
  MatrixData2 sub = submatrix(m, row, column);
  return determinant(sub);
}

float minor(const MatrixData4 &m, size_t row, size_t column) {
  MatrixData3 sub = submatrix(m, row, column);
  return determinant(sub);
}

float cofactor(const MatrixData3 &m, size_t row, size_t column) {
  float mi = minor(m, row, column);
  if ((row + column) % 2 == 1) {
    return -mi;
  }
  return mi;
}

float cofactor(const MatrixData4 &m, size_t row, size_t column) {
  float mi = minor(m, row, column);
  if ((row + column) % 2 == 1) {
    return -mi;
  }
  return mi;
}

MatrixData4 get_inverse(const MatrixData4 &m) {
  float det = determinant(m);

  MatrixData4 out;
  for (size_t row = 0; row < 4; ++row) {
    for (size_t col = 0; col < 4; ++col) {
      float c = cofactor(m, row, col);
      out[col][row] = c / det;
    }
  }
  return out;
}

MatrixData2 submatrix(const MatrixData3 &m, size_t row, size_t column) {
  MatrixData2 out;

  size_t out_r = 0;
  for (size_t r = 0; r < 3; ++r) {
    if (r == row) {
      continue;
    }
    size_t out_c = 0;
    for (size_t c = 0; c < 3; ++c) {
      if (c == column) {
        continue;
      }
      out[out_r][out_c] = m[r][c];
      out_c++;
    }
    out_r++;
  }
  return out;
}

MatrixData3 submatrix(const MatrixData4 &m, size_t row, size_t column) {
  MatrixData3 out;

  size_t out_r = 0;
  for (size_t r = 0; r < 4; ++r) {
    if (r == row) {
      continue;
    }
    size_t out_c = 0;
    for (size_t c = 0; c < 4; ++c) {
      if (c == column) {
        continue;
      }
      out[out_r][out_c] = m[r][c];
      out_c++;
    }
    out_r++;
  }
  return out;
}
