#include "../color.h"

#include "../tuple.h"
#include "gtest/gtest.h"

class ColorTest : public ::testing::Test {};

TEST(ColorTest, Color) {
  // Scenario: Colors are (red, green, blue) tuples

  // Given c ← color(-0.5, 0.4, 1.7)
  const auto c = Color(-0.5, 0.4, 1.7);

  // Then c.red = -0.5
  EXPECT_NEAR(-0.5, c.x, EPSILON);

  // And c.green = 0.4
  EXPECT_NEAR(0.4, c.y, EPSILON);

  // And c.blue = 1.7
  EXPECT_NEAR(1.7, c.z, EPSILON);
}

TEST(ColorTest, AddingColors) {
  // Scenario: Adding colors

  // Given c1 ← color(0.9, 0.6, 0.75)
  const auto c1 = Color(0.9, 0.6, 0.75);

  // And c2 ← color(0.7, 0.1, 0.25)
  const auto c2 = Color(0.7, 0.1, 0.25);

  // Then c1 + c2 = color(1.6, 0.7, 1.0)
  EXPECT_EQ(Color(1.6, 0.7, 1.0), c1 + c2);
}

TEST(ColorTest, SubtracingColors) {
  // Scenario: Subtracting colors

  // Given c1 ← color(0.9, 0.6, 0.75)
  const auto c1 = Color(0.9, 0.6, 0.75);

  // And c2 ← color(0.7, 0.1, 0.25)
  const auto c2 = Color(0.7, 0.1, 0.25);

  // Then c1 - c2 = color(0.2, 0.5, 0.5)
  EXPECT_EQ(Color(0.2, 0.5, 0.5), c1 - c2);
}

TEST(ColorTest, MultiplyColorByScalar) {
  // Scenario: Multiplying a color by a scalar

  // Given c ← color(0.2, 0.3, 0.4)
  const auto c = Color(0.2, 0.3, 0.4);

  // Then c * 2 = color(0.4, 0.6, 0.8)
  EXPECT_EQ(Color(0.4, 0.6, 0.8), c * 2);
}

TEST(ColorTest, MultiplyColors) {
  // Scenario: Multiplying colors

  // Given c1 ← color(1, 0.2, 0.4)
  const auto c1 = Color(1, 0.2, 0.4);

  // And c2 ← color(0.9, 1, 0.1)
  const auto c2 = Color(0.9, 1, 0.1);

  // Then c1 * c2 = color(0.9, 0.2, 0.04)
  EXPECT_EQ(Color(0.9, 0.2, 0.04), c1 * c2);
}