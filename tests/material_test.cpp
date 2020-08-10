#include "../core/material.h"

#include "../core/color.h"
#include "../core/light.h"
#include "../core/matrix.h"
#include "../core/tuple.h"
#include "../shapes/sphere.h"
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

  auto r = m.lighting(nullptr, l, p, e, n, false);
  EXPECT_EQ(Color(1.9, 1.9, 1.9), r);
}

TEST(Material, EyeBetween45) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, SQRT2_2, -SQRT2_2);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1));

  auto r = m.lighting(nullptr, l, p, e, n, false);
  EXPECT_EQ(Color(1.0, 1.0, 1.0), r);
}

TEST(Material, EyeOpposite45) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, 0, -1);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 10, -10), Color(1, 1, 1));

  auto r = m.lighting(nullptr, l, p, e, n, false);
  EXPECT_EQ(Color(0.7364, 0.7364, 0.7364), r);
}

TEST(Material, EyeInPath) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, -SQRT2_2, -SQRT2_2);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 10, -10), Color(1, 1, 1));

  auto r = m.lighting(nullptr, l, p, e, n, false);
  EXPECT_TRUE(color_is_near(Color(1.6364, 1.6364, 1.6364), r, 0.0001));
}

TEST(Material, LightBehind) {
  auto m = Material();
  auto p = Tuple::point(0, 0, 0);

  auto e = Tuple::vector(0, 0, -1);
  auto n = Tuple::vector(0, 0, -1);
  auto l = PointLight(Tuple::point(0, 0, 10), Color(1, 1, 1));

  auto r = m.lighting(nullptr, l, p, e, n, false);
  EXPECT_TRUE(color_is_near(Color(0.1, 0.1, 0.1), r, 0.0001));
}

TEST(Material, DefaultHasReflective) {
  auto m = Material();
  EXPECT_EQ(0, m.reflective());
}

TEST(Material, DefaultTransparencyAndRefractive) {
  auto m = Material();
  EXPECT_EQ(0.0, m.transparency());
  EXPECT_EQ(1.0, m.refractive());
}

TEST(Material, LightingWithPattern){
  auto m = Material();
  auto p = StripePattern(Color(1, 1, 1), Color(0, 0, 0));
  m.set_pattern(p);
  m.set_ambient(1);
  m.set_diffuse(0);
  m.set_specular(0);

  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  s->set_material(m);

  auto eyev = Tuple::vector(0, 0, -1);
  auto normalv = Tuple::vector(0, 0, -1);

  auto light = PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1));

  auto c1 = m.lighting(s, light, Tuple::point(0.9, 0, 0), eyev, normalv, false);
  auto c2 = m.lighting(s, light, Tuple::point(1.1, 0, 0), eyev, normalv, false);
  EXPECT_EQ(Color(1, 1, 1), c1);
  EXPECT_EQ(Color(0, 0, 0), c2);
}
