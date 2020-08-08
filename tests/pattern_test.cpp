//
// Created by Brian Landers on 8/7/20.
//

#include "../color.h"
#include "../pattern.h"
#include "gtest/gtest.h"

TEST(Pattern, StripePattern) {
  auto black = Color(0, 0, 0);
  auto white = Color(1, 1, 1);

  auto pattern = StripePattern(white, black);
  EXPECT_EQ(white, pattern.a);
  EXPECT_EQ(black, pattern.b);
}

TEST(Pattern, StripePatternConstantInY) {
  auto black = Color(0, 0, 0);
  auto white = Color(1, 1, 1);

  auto pattern = StripePattern(white, black);
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 1, 0)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 2, 0)));
}

TEST(Pattern, StripePatternConstantInZ) {
  auto black = Color(0, 0, 0);
  auto white = Color(1, 1, 1);

  auto pattern = StripePattern(white, black);
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 0, 1)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 0, 2)));
}

TEST(Pattern, StripePatternAlternatesInX) {
  auto black = Color(0, 0, 0);
  auto white = Color(1, 1, 1);

  auto pattern = StripePattern(white, black);
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(0.9, 0, 0)));

  EXPECT_EQ(black, pattern.pattern_at(Tuple::point(1, 0, 0)));
  EXPECT_EQ(black, pattern.pattern_at(Tuple::point(-0.1, 0, 0)));

  EXPECT_EQ(black, pattern.pattern_at(Tuple::point(-1, 0, 0)));
  EXPECT_EQ(white, pattern.pattern_at(Tuple::point(-1.1, 0, 0)));
}

TEST(Pattern, DefaultTransform) {
  auto p = TestPattern();
  EXPECT_EQ(Matrix(IDENTITY), p.transform());
}

TEST(Pattern, Assign) {
  auto p = TestPattern();
  p.set_transform(CreateTranslation(1, 2, 3));
  EXPECT_EQ(CreateTranslation(1, 2, 3), p.transform());
}

TEST(Pattern, Gradient) {
  auto p = GradientPattern(Color(1, 1, 1), Color(0, 0, 0));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(Color(0.75, 0.75, 0.75), p.pattern_at(Tuple::point(0.25, 0, 0)));
  EXPECT_EQ(Color(0.5, 0.5, 0.5), p.pattern_at(Tuple::point(0.5, 0, 0)));
  EXPECT_EQ(Color(0.25, 0.25, 0.25), p.pattern_at(Tuple::point(0.75, 0, 0)));
}

TEST(Pattern, Ring) {
  auto p = RingPattern(Color(1, 1, 1), Color(0, 0, 0));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(1, 0, 0)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(0, 0, 1)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(0.708, 0, 0.708)));
}

TEST(Pattern, CheckersX) {
  auto p = CheckPattern(Color(1, 1, 1), Color(0, 0, 0));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0.99, 0, 0)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(1.01, 0, 0)));
}

TEST(Pattern, CheckersY) {
  auto p = CheckPattern(Color(1, 1, 1), Color(0, 0, 0));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0.99, 0)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(0, 1.01, 0)));
}

TEST(Pattern, CheckersZ) {
  auto p = CheckPattern(Color(1, 1, 1), Color(0, 0, 0));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0)));
  EXPECT_EQ(Color(1, 1, 1), p.pattern_at(Tuple::point(0, 0, 0.99)));
  EXPECT_EQ(Color(0, 0, 0), p.pattern_at(Tuple::point(0, 0, 1.01)));
}