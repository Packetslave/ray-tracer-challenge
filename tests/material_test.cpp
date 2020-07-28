#include "../material.h"

#include "../color.h"
#include "../light.h"
#include "../matrix.h"
#include "../tuple.h"
#include "gtest/gtest.h"

bool color_is_near(Color a, Color b, float abs) {
  return ((std::abs(a.x - b.x) < abs) && (std::abs(a.y - b.y) < abs) &&
          (std::abs(a.z - b.z) < abs) && (a.w == b.w));
}

TEST(Material, Default) {
  auto m = Material();
  EXPECT_EQ(Color(1, 1, 1), m.color());
  EXPECT_EQ(0.1, m.ambient());
  EXPECT_EQ(0.9, m.diffuse());
  EXPECT_EQ(0.9, m.specular());
  EXPECT_EQ(200.0, m.shininess());
}

TEST(Material, EyeBetween) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, 0, -1);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1));

  auto r = m.lighting(l, p, e, n, false);
  EXPECT_EQ(Color(1.9, 1.9, 1.9), r);
}

TEST(Material, EyeBetween45) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, SQRT2_2, -SQRT2_2);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1));

  auto r = m.lighting(l, p, e, n, false);
  EXPECT_EQ(Color(1.0, 1.0, 1.0), r);
}

TEST(Material, EyeOpposite45) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, 0, -1);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 10, -10), Color(1, 1, 1));

  auto r = m.lighting(l, p, e, n, false);
  EXPECT_EQ(Color(0.7364, 0.7364, 0.7364), r);
}

TEST(Material, EyeInPath) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, -SQRT2_2, -SQRT2_2);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 10, -10), Color(1, 1, 1));

  auto r = m.lighting(l, p, e, n, false);
  EXPECT_TRUE(color_is_near(Color(1.6364, 1.6364, 1.6364), r, 0.0001));
}

TEST(Material, LightBehind) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, 0, -1);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 0, 10), Color(1, 1, 1));

  auto r = m.lighting(l, p, e, n, false);
  EXPECT_TRUE(color_is_near(Color(0.1, 0.1, 0.1), r, 0.0001));
}

TEST(Material, DefaultHasReflective) {
  auto m = Material();
  EXPECT_EQ(0, m.reflective());
}
