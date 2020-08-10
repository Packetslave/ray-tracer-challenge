#include "../core/light.h"

#include "../core/color.h"
#include "../core/material.h"
#include "../core/tuple.h"
#include "gtest/gtest.h"

TEST(Light, PointLight) {
  auto i = Color(1, 1, 1);
  auto p = Tuple::point(0, 0, 0);
  auto l = PointLight(p, i);
  EXPECT_EQ(p, l.position());
  EXPECT_EQ(i, l.intensity());
}

TEST(Light, SurfaceInShadow) {
  auto position = Tuple::point(0, 0, 0);
  auto eyev = Tuple::vector(0, 0, -1);
  auto normalv = Tuple::vector(0, 0, -1);
  auto light = PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1));
  auto in_shadow = true;
  auto m = Material();
  auto result = m.lighting(nullptr, light, position, eyev, normalv, in_shadow);
  EXPECT_EQ(Color(0.1, 0.1, 0.1), result);
}