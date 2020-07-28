//
// Created by Brian Landers on 7/25/20.
//

#include "../tuple.h"

#include "gtest/gtest.h"

TEST(TupleTest, Point) {
  // Scenario: A tuple with w=1.0 is a point

  // Given a ← tuple(4.3, -4.2, 3.1, 1.0)
  const Tuple a(4.3, -4.2, 3.1, 1.0);

  //   Then a.x = 4.3
  EXPECT_NEAR(4.3, a.x, EPSILON);

  //     And a.y = -4.2
  EXPECT_NEAR(-4.2, a.y, EPSILON);

  //     And a.z = 3.1
  EXPECT_NEAR(3.1, a.z, EPSILON);

  //     And a.w = 1.0
  EXPECT_NEAR(1.0, a.w, EPSILON);

  //     And a is a point
  EXPECT_TRUE(a.point());

  //     And a is not a vector
  EXPECT_FALSE(a.vector());
}

TEST(TupleTest, Vector) {
  // Scenario: A tuple with w=0.0 is a vector

  // Given a ← tuple(4.3, -4.2, 3.1, 0.0)
  const Tuple a(4.3, -4.2, 3.1, 0.0);

  //   Then a.x = 4.3
  EXPECT_NEAR(4.3, a.x, EPSILON);

  //     And a.y = -4.2
  EXPECT_NEAR(-4.2, a.y, EPSILON);

  //     And a.z = 3.1
  EXPECT_NEAR(3.1, a.z, EPSILON);

  //     And a.w = 1.0
  EXPECT_NEAR(0.0, a.w, EPSILON);

  //     And a is a point
  EXPECT_FALSE(a.point());

  //     And a is not a vector
  EXPECT_TRUE(a.vector());
}

TEST(TupleTest, PointFactory) {
  // Scenario: point() creates tuples with w=1

  // Given p ← point(4, -4, 3)
  const auto p = Tuple::point(4, -4, 3);

  // Then p = tuple(4, -4, 3, 1)
  EXPECT_EQ(Tuple(4, -4, 3, 1.0), p);
}

TEST(TupleTest, TupleFactory) {
  // Scenario: vector() creates tuples with w=0
  // Given v ← vector(4, -4, 3)
  const auto v = Tuple::vector(4, -4, 3);

  // Then v = tuple(4, -4, 3, 0)
  EXPECT_EQ(Tuple(4, -4, 3, 0), v);
}

TEST(TupleTest, Addition) {
  // Scenario: Adding two tuples
  // Given a1 ← tuple(3, -2, 5, 1)
  const auto a1 = Tuple(3, -2, 5, 1);

  // And a2 ← tuple(-2, 3, 1, 0)
  const auto a2 = Tuple(-2, 3, 1, 0);

  // Then a1 + a2 = tuple(1, 1, 6, 1)
  EXPECT_EQ(Tuple(1, 1, 6, 1), a1 + a2);
}

TEST(TupleTest, SubtractPoints) {
  // Scenario: Subtracting two points
  // Given p1 ← point(3, 2, 1)
  const auto p1 = Tuple::point(3, 2, 1);

  // And p2 ← point(5, 6, 7)
  const auto p2 = Tuple::point(5, 6, 7);

  // Then p1 - p2 = vector(-2, -4, -6)
  EXPECT_EQ(Tuple::vector(-2, -4, -6), p1 - p2);
}

TEST(TupleTest, SubtractVectorFromPoint) {
  // Scenario: Subtracting a vector from a point
  // Given p ← point(3, 2, 1)
  const auto p = Tuple::point(3, 2, 1);

  // And v ← vector(5, 6, 7)
  const auto v = Tuple::vector(5, 6, 7);

  // Then p - v = point(-2, -4, -6)
  EXPECT_EQ(Tuple::point(-2, -4, -6), p - v);
}

TEST(TupleTest, SubtractTwoVectors) {
  // Scenario: Subtracting two vectors
  // Given v1 ← vector(3, 2, 1)
  const auto v1 = Tuple::vector(3, 2, 1);

  // And v2 ← vector(5, 6, 7)
  const auto v2 = Tuple::vector(5, 6, 7);

  // Then v1 - v2 = vector(-2, -4, -6)
  EXPECT_EQ(Tuple::vector(-2, -4, -6), v1 - v2);
}

TEST(TupleTest, Negation) {
  // Scenario: Negating a tuple
  // Given a ← tuple(1, -2, 3, -4)
  const auto a = Tuple(1, -2, 3, -4);

  // Then -a = tuple(-1, 2, -3, 4)
  EXPECT_EQ(Tuple(-1, 2, -3, 4), -a);
}

TEST(TupleTest, ScalarMultiplication1) {
  // Scenario: Multiplying a tuple by a scalar
  // Given a ← tuple(1, -2, 3, -4)
  const auto a = Tuple(1, -2, 3, -4);

  // Then a * 3.5 = tuple(3.5, -7, 10.5, -14)
  EXPECT_EQ(Tuple(3.5, -7, 10.5, -14), a * 3.5);
}

TEST(TupleTest, ScalarMultiplication2) {
  // Scenario: Multiplying a tuple by a fraction
  // Given a ← tuple(1, -2, 3, -4)
  const auto a = Tuple(1, -2, 3, -4);

  // Then a * 0.5 = tuple(0.5, -1, 1.5, -2)
  EXPECT_EQ(Tuple(0.5, -1, 1.5, -2), a * 0.5);
}

TEST(TupleTest, ScalarDivision) {
  // Scenario: Dividing a tuple by a scalar
  // Given a ← tuple(1, -2, 3, -4)
  const auto a = Tuple(1, -2, 3, -4);

  // Then a / 2 = tuple(0.5, -1, 1.5, -2)
  EXPECT_EQ(Tuple(0.5, -1, 1.5, -2), a / 2);
}

TEST(TupleTest, Magnitude1) {
  // Scenario: Computing the magnitude of vector(1, 0, 0)

  // Given v ← vector(1, 0, 0)
  const auto v = Tuple::vector(1, 0, 0);

  // Then v.magnitude() = 1
  EXPECT_EQ(1, v.magnitude());
}

TEST(TupleTest, Magnitude2) {
  // Scenario: Computing the magnitude of vector(0, 1, 0)

  // Given v ← vector(0, 1, 0)
  const auto v = Tuple::vector(0, 1, 0);

  // Then v.magnitude() = 1
  EXPECT_EQ(1, v.magnitude());
}

TEST(TupleTest, Magnitude3) {
  // Scenario: Computing the magnitude of vector(0, 0, 1)

  // Given v ← vector(0, 0, 1)
  const auto v = Tuple::vector(0, 0, 1);

  // Then v.magnitude() = 1
  EXPECT_EQ(1, v.magnitude());
}

TEST(TupleTest, Magnitude4) {
  // Scenario: Computing the magnitude of vector(1, 2, 3)

  // Given v ← vector(1, 2, 3)
  const auto v = Tuple::vector(1, 2, 3);

  // Then v.magnitude() = √14
  EXPECT_NEAR(sqrt(14), v.magnitude(), EPSILON);
}

TEST(TupleTest, Magnitude5) {
  // Scenario: Computing the magnitude of vector(-1, -2, -3)

  // Given v ← vector(-1, -2, -3)
  const auto v = Tuple::vector(-1, -2, -3);

  // Then v.magnitude() = √14
  EXPECT_NEAR(sqrt(14), v.magnitude(), EPSILON);
}

TEST(TupleTest, Normalization1) {
  // Scenario: Normalizing vector(4, 0, 0) gives (1, 0, 0)
  // Given v ← vector(4, 0, 0)
  const auto v = Tuple::vector(4, 0, 0);

  // Then v.normalize() = vector(1, 0, 0)
  EXPECT_EQ(Tuple::vector(1, 0, 0), v.normalize());
}

TEST(TupleTest, Normalization2) {
  // Scenario: Normalizing vector(1, 2, 3)

  // Given v ← vector(1, 2, 3)
  const auto v = Tuple::vector(1, 2, 3);

  // Then v.normalize() = approximately vector(0.26726, 0.53452, 0.80178)
  EXPECT_EQ(Tuple::vector(0.26726, 0.53452, 0.80178), v.normalize());
}

TEST(TupleTest, Normalization3) {
  // Scenario: The magnitude of a normalized vector

  // Given v ← vector(1, 2, 3)
  const auto v = Tuple::vector(1, 2, 3);

  // When norm ← v.normalize()
  const auto norm = v.normalize();

  // Then magnitude(norm) = 1
  EXPECT_NEAR(1, norm.magnitude(), EPSILON);
}

TEST(TupleTest, DotProduct) {
  // Scenario: The dot product of two tuples

  // Given a ← vector(1, 2, 3)
  const auto a = Tuple::vector(1, 2, 3);

  // And b ← vector(2, 3, 4)
  const auto b = Tuple::vector(2, 3, 4);

  // Then dot(a, b) = 20
  EXPECT_EQ(20, dot(a, b));
}

TEST(TupleTest, CrossProduct) {
  // Scenario: The cross product of two vectors

  // Given a ← vector(1, 2, 3)
  const auto a = Tuple::vector(1, 2, 3);

  // And b ← vector(2, 3, 4)
  const auto b = Tuple::vector(2, 3, 4);

  // Then cross(a, b) = vector(-1, 2, -1)
  EXPECT_EQ(Tuple::vector(-1, 2, -1), cross(a, b));

  // And cross(b, a) = vector(1, -2, 1)
  EXPECT_EQ(Tuple::vector(1, -2, 1), cross(b, a));
}

TEST(TupleTest, ReflectVector45) {
  auto v = Tuple::vector(1, -1, 0);
  auto n = Tuple::vector(0, 1, 0);
  auto r = v.reflect(n);
  EXPECT_EQ(Tuple::vector(1, 1, 0), r);
}

TEST(TupleTest, ReflectVectorSlanted) {
  auto v = Tuple::vector(0, -1, 0);
  auto n = Tuple::vector(SQRT2_2, SQRT2_2, 0);
  auto r = v.reflect(n);
  EXPECT_EQ(Tuple::vector(1, 0, 0), r);
}
