//
// Created by Brian Landers on 2018-12-14.
//

//
// Created by Brian Landers on 2018-12-07.
//
#include "../core/matrix.h"

#include <iostream>

#include "../core/tuple.h"
#include "gtest/gtest.h"
#include "test_common.h"

TEST(Matrix, Create4x4) {
  Matrix m{MatrixData4{{{1, 2, 3, 4},
                        {5.5, 6.5, 7.5, 8.5},
                        {9, 10, 11, 12},
                        {13.5, 14.5, 15.5, 16.5}}}};
  ASSERT_EQ(m.get(0, 0), 1);
  ASSERT_EQ(m.get(0, 3), 4);
  ASSERT_EQ(m.get(1, 0), 5.5);
  ASSERT_EQ(m.get(1, 2), 7.5);
  ASSERT_EQ(m.get(2, 2), 11);
  ASSERT_EQ(m.get(3, 0), 13.5);
  ASSERT_EQ(m.get(3, 2), 15.5);
}

TEST(Matrix, Equality) {
  Matrix a{
      MatrixData4{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 8, 7, 6}, {5, 4, 3, 2}}}};

  Matrix b{
      MatrixData4{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 8, 7, 6}, {5, 4, 3, 2}}}};
  ASSERT_EQ(a, b);
}

TEST(Matrix, Inequality) {
  Matrix a{
      MatrixData4{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 8, 37, 6}, {5, 4, 3, 2}}}};
  Matrix b{
      MatrixData4{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 8, 7, 6}, {5, 4, 3, 2}}}};
  ASSERT_NE(a, b);
}

TEST(Matrix, Multiply) {
  Matrix a{
      MatrixData4{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 8, 7, 6}, {5, 4, 3, 2}}}};
  Matrix b{
      MatrixData4{{{-2, 1, 2, 3}, {3, 2, 1, -1}, {4, 3, 6, 5}, {1, 2, 7, 8}}}};
  Matrix c{MatrixData4{{{20, 22, 50, 48},
                        {44, 54, 114, 108},
                        {40, 58, 110, 102},
                        {16, 26, 46, 42}}}};

  Matrix d = a * b;
  ASSERT_EQ(d, c);
}

TEST(Matrix, MultipleByTuple) {
  Matrix a{MatrixData4{{
      {1, 2, 3, 4},
      {2, 4, 4, 2},
      {8, 6, 4, 1},
      {0, 0, 0, 1},
  }}};
  Tuple b(1, 2, 3, 1);
  Tuple c = a * b;
  ASSERT_EQ(Tuple(18, 24, 33, 1), c);
}

TEST(Matrix, CreateIdentity) {
  Matrix a{
      MatrixData4{{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}}};
  Matrix b = Matrix(IDENTITY);
  ASSERT_EQ(a, b);
}

TEST(Matrix, MultiplyByIdentity) {
  Matrix a = Matrix(IDENTITY);
  Matrix b{
      MatrixData4{{{0, 1, 2, 4}, {1, 2, 4, 8}, {2, 4, 8, 16}, {4, 8, 16, 32}}}};
  Matrix c = b * a;

  ASSERT_EQ(b, c);
}

TEST(Matrix, Transpose) {
  Matrix a{
      MatrixData4{{{0, 9, 3, 0}, {9, 8, 0, 8}, {1, 8, 5, 3}, {0, 0, 5, 8}}}};
  Matrix b{
      MatrixData4{{{0, 9, 1, 0}, {9, 8, 8, 0}, {3, 0, 5, 5}, {0, 8, 3, 8}}}};
  Matrix c = a.transpose();
  ASSERT_EQ(b, c);
}

TEST(Matrix, TransposeIdentity) {
  Matrix a = Matrix(IDENTITY);
  Matrix b = a.transpose();
  ASSERT_EQ(a, b);
}

TEST(Matrix, Determinant2x2) {
  MatrixData2 a{{
      {1, 5},
      {-3, 2},
  }};
  ASSERT_EQ(17, determinant(a));
}

TEST(Matrix, Determinant3x3) {
  MatrixData3 a{{
      {1, 2, 6},
      {-5, 8, -4},
      {2, 6, 4},
  }};
  ASSERT_EQ(56, cofactor(a, 0, 0));
  ASSERT_EQ(12, cofactor(a, 0, 1));
  ASSERT_EQ(-46, cofactor(a, 0, 2));
  ASSERT_EQ(-196, determinant(a));
}
//
TEST(Matrix, Determinant4x4) {
  MatrixData4 a{{{-2, -8, 3, 5}, {-3, 1, 7, 3}, {1, 2, -9, 6}, {-6, 7, 7, -9}}};

  EXPECT_EQ(690, cofactor(a, 0, 0));
  EXPECT_EQ(447, cofactor(a, 0, 1));
  EXPECT_EQ(210, cofactor(a, 0, 2));
  EXPECT_EQ(51, cofactor(a, 0, 3));
  EXPECT_EQ(-4071, determinant(a));
}

template <class T>
bool matrix_data_is_near(T a, T b) {
  for (size_t i = 0; i < a.size(); ++i) {
    for (size_t j = 0; j < b.size(); ++j) {
      if (abs(a[i][j] - b[i][j]) > EPSILON) {
        return false;
      }
    }
  }
  return true;
}

TEST(Matrix, Submatrix3x3) {
  MatrixData4 a{{
      {1, 5, 0},
      {-3, 2, 7},
      {0, 6, -3},
  }};
  MatrixData3 b{{
      {-3, 2},
      {0, 6},
  }};
  MatrixData3 c = submatrix(a, 0, 2);
  ASSERT_TRUE(matrix_data_is_near<MatrixData3>(b, c));
}

TEST(Matrix, Submatrix4x4) {
  MatrixData4 a{{
      {-6, 1, 1, 6},
      {-8, 5, 8, 6},
      {-1, 0, 8, 2},
      {-7, 1, -1, 1},
  }};
  MatrixData3 b{{
      {-6, 1, 6},
      {-8, 8, 6},
      {-7, -1, 1},
  }};
  MatrixData3 c = submatrix(a, 2, 1);
  ASSERT_TRUE(matrix_data_is_near(b, c));
}

TEST(Matrix, Minor3x3) {
  MatrixData3 a{{
      {3, 5, 0},
      {2, -1, -7},
      {6, -1, 5},
  }};
  MatrixData2 b = submatrix(a, 1, 0);
  ASSERT_EQ(25, determinant(b));
  ASSERT_EQ(25, minor(a, 1, 0));
}

TEST(Matrix, Cofactor) {
  MatrixData3 a{{
      {3, 5, 0},
      {2, -1, -7},
      {6, -1, 5},
  }};
  ASSERT_EQ(-12, minor(a, 0, 0));
  ASSERT_EQ(-12, cofactor(a, 0, 0));
  ASSERT_EQ(25, minor(a, 1, 0));
  ASSERT_EQ(-25, cofactor(a, 1, 0));
}

TEST(Matrix, IsInvertible) {
  Matrix a{MatrixData4{{
      {6, 4, 4, 4},
      {5, 5, 7, 6},
      {4, -9, 3, -7},
      {9, 1, 7, -6},
  }}};
  ASSERT_EQ(-2120, determinant(*(a.TEST_get_data())));
  ASSERT_TRUE(a.is_invertible());

  Matrix b{MatrixData4{{
      {-4, 2, -2, -3},
      {9, 6, 2, 6},
      {0, -5, 1, -5},
      {0, 0, 0, 0},
  }}};
  ASSERT_EQ(0, determinant(*(b.TEST_get_data())));
  ASSERT_FALSE(b.is_invertible());
}

TEST(Matrix, Inverse1) {
  Matrix a{MatrixData4{{
      {-5, 2, 6, -8},
      {1, -5, 1, 8},
      {7, 7, -6, -7},
      {1, -3, 7, 4},
  }}};
  Matrix b{MatrixData4{{
      {0.21805, 0.45113, 0.24060, -0.04511},
      {-0.80827, -1.45677, -0.44361, 0.52068},
      {-0.07895, -0.22368, -0.05263, 0.19737},
      {-0.52256, -0.81391, -0.30075, 0.30639},
  }}};
  const auto data = a.TEST_get_data();
  ASSERT_EQ(532.0, determinant(*data));
  ASSERT_EQ(-160.0, cofactor(*data, 2, 3));
  ASSERT_NEAR(-160.0 / 532.0, b.get(3, 2), EPSILON);
  ASSERT_EQ(105.0, cofactor(*data, 3, 2));
  ASSERT_NEAR(105.0 / 532.0, b.get(2, 3), EPSILON);
  ASSERT_TRUE(matrix_is_near(b, a.inverse(), EPSILON));
}

TEST(Matrix, Inverse2) {
  Matrix a{MatrixData4{{
      {8, -5, 9, 2},
      {7, 5, 6, 1},
      {-6, 0, 9, 6},
      {-3, 0, -9, -4},
  }}};
  Matrix b{MatrixData4{{
      {-0.15385, -0.15385, -0.28205, -0.53846},
      {-0.07692, 0.12308, 0.02564, 0.03077},
      {0.35897, 0.35897, 0.43590, 0.92308},
      {-0.69231, -0.69231, -0.76923, -1.92308},
  }}};
  ASSERT_TRUE(matrix_is_near(b, a.inverse(/* cache */ false), EPSILON));
}

TEST(Matrix, Inverse3) {
  Matrix a{MatrixData4{{
      {9, 3, 0, 9},
      {-5, -2, -6, -3},
      {-4, 9, 6, 4},
      {-7, 6, 6, 2},
  }}};
  Matrix b{MatrixData4{{
      {-0.04074, -0.07778, 0.14444, -0.22222},
      {-0.07778, 0.03333, 0.36667, -0.33333},
      {-0.02901, -0.14630, -0.10926, 0.12963},
      {0.17778, 0.06667, -0.26667, 0.33333},
  }}};
  ASSERT_TRUE(matrix_is_near(b, a.inverse(/*cache */ false), EPSILON));
}

TEST(Matrix, MultiplyByInverse) {
  Matrix a{MatrixData4{{
      {3, -9, 7, 3},
      {3, -8, 2, -9},
      {-4, 4, 4, 1},
      {-6, 5, -1, 1},
  }}};
  Matrix b{MatrixData4{{
      {8, 2, 2, 2},
      {3, -1, 7, 0},
      {7, 0, 5, 4},
      {6, -2, 0, 5},
  }}};
  Matrix c = a * b;
  Matrix d = c * b.inverse(/* cache */ false);
  ASSERT_TRUE(matrix_is_near(d, a, 0.00001));
}

// // // ---------------- Matrix Transforms ----------------

TEST(Transform, MultiplyTransformByTuple) {
  Matrix a = CreateTranslation(5, -3, 2);
  Tuple b = Tuple::point(-3, 4, 5);
  Tuple c = a * b;
  ASSERT_EQ(Tuple::point(2, 1, 7), c);
}

TEST(Transform, MultiplyTransformInverseByTuple) {
  Matrix a = CreateTranslation(5, -3, 2);
  Matrix a2 = a.inverse(/* cache */ false);
  Tuple b = Tuple::point(-3, 4, 5);
  Tuple c = a2 * b;
  ASSERT_EQ(Tuple::point(-8, 7, 3), c);
}

TEST(Transform, MultiplyTransformByVector) {
  Matrix a = CreateTranslation(5, -3, 2);
  Tuple b = Tuple::vector(-3, 4, 5);
  Tuple c = a * b;
  ASSERT_EQ(b, c);
}

TEST(Transform, ScaleTuple) {
  Matrix a = CreateScaling(2, 3, 4);
  Tuple b = Tuple::point(-4, 6, 8);
  Tuple c = a * b;
  ASSERT_EQ(Tuple::point(-8, 18, 32), c);
}

TEST(Transform, ScaleVector) {
  Matrix a = CreateScaling(2, 3, 4);
  Tuple b = Tuple::vector(-4, 6, 8);
  Tuple c = a * b;
  ASSERT_EQ(Tuple::vector(-8, 18, 32), c);
}

TEST(Transform, MultiplyByScalingInverse) {
  Matrix a = CreateScaling(2, 3, 4);
  Matrix b = a.inverse(/* cache */ false);
  Tuple c = Tuple::vector(-4, 6, 8);
  Tuple d = b * c;
  ASSERT_EQ(Tuple::vector(-2, 2, 2), d);
}

TEST(Transform, Reflection) {
  Matrix a = CreateScaling(-1, 1, 1);
  Tuple b = Tuple::point(2, 3, 4);
  Tuple c = a * b;
  ASSERT_EQ(Tuple::point(-2, 3, 4), c);
}

TEST(Transform, RotateX) {
  auto p = Tuple::point(0, 1, 0);
  auto half = CreateRotationX(PI_4);
  auto full = CreateRotationX(PI_2);
  ASSERT_TRUE(
      point_is_near(Tuple::point(0, SQRT2_2, SQRT2_2), half * p, 0.001));
  ASSERT_TRUE(point_is_near(Tuple::point(0, 0, 1), full * p, 0.001));
}

TEST(Transform, RotateXInverse) {
  auto p = Tuple::point(0, 1, 0);
  auto half = CreateRotationX(PI_4);
  auto inv = half.inverse(/* cache */ false);
  ASSERT_TRUE(
      point_is_near(Tuple::point(0, SQRT2_2, -1 * SQRT2_2), inv * p, 0.001));
}

TEST(Transform, RotateY) {
  auto p = Tuple::point(0, 0, 1);
  auto half = CreateRotationY(PI_4);
  auto full = CreateRotationY(PI_2);
  ASSERT_TRUE(
      point_is_near(Tuple::point(SQRT2_2, 0, SQRT2_2), half * p, 0.001));
  ASSERT_TRUE(point_is_near(Tuple::point(1, 0, 0), full * p, 0.001));
}

TEST(Transform, RotateZ) {
  auto p = Tuple::point(0, 1, 0);
  auto half = CreateRotationZ(PI_4);
  auto full = CreateRotationZ(PI_2);
  ASSERT_TRUE(
      point_is_near(Tuple::point(-1 * SQRT2_2, SQRT2_2, 0), half * p, 0.001));
  ASSERT_TRUE(point_is_near(Tuple::point(-1, 0, 0), full * p, 0.001));
}

TEST(Transform, ShearXY) {
  auto transform = CreateShearing(1, 0, 0, 0, 0, 0);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(5, 3, 4), transform * p);
}

TEST(Transform, ShearXZ) {
  auto transform = CreateShearing(0, 1, 0, 0, 0, 0);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(6, 3, 4), transform * p);
}

TEST(Transform, ShearYX) {
  auto transform = CreateShearing(0, 0, 1, 0, 0, 0);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(2, 5, 4), transform * p);
}

TEST(Transform, ShearYZ) {
  auto transform = CreateShearing(0, 0, 0, 1, 0, 0);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(2, 7, 4), transform * p);
}

TEST(Transform, ShearZX) {
  auto transform = CreateShearing(0, 0, 0, 0, 1, 0);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(2, 3, 6), transform * p);
}

TEST(Transform, ShearZY) {
  auto transform = CreateShearing(0, 0, 0, 0, 0, 1);
  auto p = Tuple::point(2, 3, 4);
  ASSERT_EQ(Tuple::point(2, 3, 7), transform * p);
}

TEST(Transform, Chained) {
  auto p = Tuple::point(1, 0, 1);
  auto A = CreateRotationX(PI_2);
  auto B = CreateScaling(5, 5, 5);
  auto C = CreateTranslation(10, 5, 7);

  auto p2 = A * p;
  ASSERT_EQ(Tuple::point(1, -1, 0), p2);

  auto p3 = B * p2;
  ASSERT_EQ(Tuple::point(5, -5, 0), p3);

  auto p4 = C * p3;
  ASSERT_EQ(Tuple::point(15, 0, 7), p4);
}

TEST(Transform, Chained2) {
  auto p = Tuple::point(1, 0, 1);
  auto A = CreateRotationX(PI_2);
  auto B = CreateScaling(5, 5, 5);
  auto C = CreateTranslation(10, 5, 7);

  auto T = C * B * A;
  ASSERT_EQ(Tuple::point(15, 0, 7), T * p);
}