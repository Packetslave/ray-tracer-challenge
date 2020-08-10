//
// Created by Brian Landers on 7/27/20.
//

#include "../shapes/shape.h"

#include "../core/matrix.h"
#include "../shapes/group.h"
#include "../shapes/sphere.h"
#include "gtest/gtest.h"

TEST(ShapeTest, DefaultTransformation) {
  auto s = TestShape();
  EXPECT_EQ(Matrix(IDENTITY), s.transform());
}

TEST(ShapeTest, SetTransform) {
  auto s = TestShape();
  s.set_transform(CreateTranslation(2, 3, 4));
  EXPECT_EQ(CreateTranslation(2, 3, 4), s.transform());
}

TEST(ShapeTest, DefaultMaterial) {
  auto s = TestShape();
  auto exp = Material();
  auto m = s.material();
  EXPECT_EQ(exp, *m);
}

TEST(ShapeTest, SetMaterial) {
  auto s = TestShape();
  auto m = Material();
  m.set_ambient(1);
  s.set_material(m);
  EXPECT_EQ(m, *(s.material()));
}

TEST(ShapeTest, IntersectScaled) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = TestShape();
  s.set_transform(CreateScaling(2, 2, 2));
  auto xs = s.intersects(r);
  EXPECT_EQ(Tuple::point(0, 0, -2.5), s.saved_ray().origin());
  EXPECT_EQ(Tuple::vector(0, 0, 0.5), s.saved_ray().direction());
}

TEST(ShapeTest, IntersectTranslated) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = TestShape();
  s.set_transform(CreateTranslation(5, 0, 0));
  auto xs = s.intersects(r);
  EXPECT_EQ(Tuple::point(-5, 0, -5), s.saved_ray().origin());
  EXPECT_EQ(Tuple::vector(0, 0, 1), s.saved_ray().direction());
}

TEST(ShapeTest, NormalTranslated) {
  auto s = TestShape();
  s.set_transform(CreateTranslation(0, 1, 0));
  auto n = s.normal_at(Tuple::point(0, 1.70711, -0.70711));
  EXPECT_EQ(Tuple::vector(0, 0.70711, -0.70711), n);
}

TEST(ShapeTest, NormalTransformed) {
  auto s = TestShape();
  s.set_transform(CreateScaling(1, 0.5, 1) * CreateRotationZ(PI_5));
  auto n = s.normal_at(Tuple::point(0, SQRT2_2, -SQRT2_2));
  EXPECT_EQ(Tuple::vector(0, 0.97014, -0.24254), n);
}
