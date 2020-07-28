//
// Created by Brian Landers on 2018-12-14.
//

#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

//#include "folly/container/F14Map.h"

#include "tuple.h"
#include "absl/container/flat_hash_map.h"

using MatrixData4 = std::array<std::array<double, 4>, 4>;
using MatrixData3 = std::array<std::array<double, 3>, 3>;
using MatrixData2 = std::array<std::array<double, 2>, 2>;

using MatrixCache = absl::flat_hash_map<MatrixData4, MatrixData4>;

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
double determinant(const MatrixData2 &m);
double determinant(const MatrixData3 &m);
double determinant(const MatrixData4 &m);
double cofactor(const MatrixData4 &m, size_t row, size_t column);
double cofactor(const MatrixData3 &m, size_t row, size_t column);
double cofactor(const MatrixData2 &m, size_t row, size_t column);
MatrixData3 submatrix(const MatrixData4 &m, size_t row, size_t column);
MatrixData2 submatrix(const MatrixData3 &m, size_t row, size_t column);
MatrixData4 get_inverse(const MatrixData4 &m);

class Matrix {
 public:
  static Matrix cache_lookup(const MatrixData4 &key);

  explicit Matrix();

  explicit Matrix(const MatrixData4 &values);

  // For testing only
  [[nodiscard]] const MatrixData4 *TEST_get_data() const;

  void print();

  [[nodiscard]] double get(const size_t row, const size_t column) const;

  void set(const size_t row, const size_t column, const double value);

  [[nodiscard]] static size_t size() ;

  [[nodiscard]] Matrix transpose() const;

  [[nodiscard]] bool is_invertible() const;

  Matrix inverse(bool /* unused */ = true);

 private:
  MatrixData4 data_ = MatrixData4{{
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0},
  }};
};

bool operator!=(Matrix const& a, Matrix const& b);
bool operator==(Matrix const& a, Matrix const& b);

Matrix operator*(const Matrix &a, const Matrix &b);
Tuple operator*(const Matrix &a, const Tuple &b);

Matrix CreateTranslation(const double x, const double y, const double z);

Matrix CreateScaling(const double x, const double y, const double z);

Matrix CreateRotationX(const double radians);

Matrix CreateRotationY(const double radians);

Matrix CreateRotationZ(const double radians);

Matrix CreateShearing(const double xy, const double xz, const double yx,
                      const double yz, const double zx, const double zy);

double determinant(const MatrixData2 &m);
double determinant(const MatrixData3 &m);
double determinant(const MatrixData4 &m);

double minor(const MatrixData3 &m, size_t row, size_t column);
double minor(const MatrixData4 &m, size_t row, size_t column);

double cofactor(const MatrixData3 &m, size_t row, size_t column);
double cofactor(const MatrixData4 &m, size_t row, size_t column);

MatrixData4 get_inverse(const MatrixData4 &m);

MatrixData2 submatrix(const MatrixData3 &m, size_t row, size_t column);
MatrixData3 submatrix(const MatrixData4 &m, size_t row, size_t column);