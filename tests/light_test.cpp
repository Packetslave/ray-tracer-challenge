#include "../core/light.h"

#include "../core/color.h"
#include "../core/material.h"
#include "../core/tuple.h"
#include "../core/world.h"
#include "gtest/gtest.h"
#include "../core/light.h"

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
  auto in_shadow = 0.0;
  auto m = Material();
  auto result = m.lighting(nullptr, light, position, eyev, normalv, in_shadow);
  EXPECT_EQ(Color(0.1, 0.1, 0.1), result);
}

TEST(Light, IntensityAt) {
  auto w = World::default_world();
  auto light = w.light();

  EXPECT_NEAR(1.0, light->intensity_at(Tuple::point(0, 1.0001, 0), &w), EPSILON);
  EXPECT_NEAR(1.0, light->intensity_at(Tuple::point(-1.0001, 0, 0), &w), EPSILON);
  EXPECT_NEAR(1.0, light->intensity_at(Tuple::point(0, 0, -1.0001), &w), EPSILON);
  EXPECT_NEAR(0.0, light->intensity_at(Tuple::point(0, 0, 1.0001), &w), EPSILON);
  EXPECT_NEAR(0.0, light->intensity_at(Tuple::point(1.0001, 0, 0), &w), EPSILON);
  EXPECT_NEAR(0.0, light->intensity_at(Tuple::point(0, -1.0001, 0), &w), EPSILON);
  EXPECT_NEAR(0.0, light->intensity_at(Tuple::point(0, 0, 0), &w), EPSILON);
}

TEST(AreaLight, Create) {
  auto corner = Tuple::point(0, 0, 0);
  auto v1 = Tuple::vector(2, 0, 0);
  auto v2 = Tuple::vector(0, 0, 1);
  auto light = AreaLight(corner, v1, 4, v2, 2, Color(1, 1, 1));
  EXPECT_EQ(corner, light.corner());
  EXPECT_EQ(Tuple::vector(0.5, 0, 0), light.uvec());
  EXPECT_EQ(4, light.usteps());
  EXPECT_EQ(Tuple::vector(0, 0, 0.5), light.vvec());
  EXPECT_EQ(2, light.vsteps());
  EXPECT_EQ(8, light.samples());
  EXPECT_EQ(Tuple::point(1, 0, 0.5), light.position());
}

TEST(AreaLight, PointOn) {
  auto corner = Tuple::point(0, 0, 0);
  auto v1 = Tuple::vector(2, 0, 0);
  auto v2 = Tuple::vector(0, 0, 1);
  auto light = AreaLight(corner, v1, 4, v2, 2, Color(1, 1, 1));
  EXPECT_EQ(corner, light.corner());

  EXPECT_EQ(Tuple::point(0.25, 0, 0.25), light.point_on(0, 0));
  EXPECT_EQ(Tuple::point(0.75, 0, 0.25), light.point_on(1, 0));
  EXPECT_EQ(Tuple::point(0.25, 0, 0.75), light.point_on(0, 1));
  EXPECT_EQ(Tuple::point(1.25, 0, 0.25), light.point_on(2, 0));
  EXPECT_EQ(Tuple::point(1.75, 0, 0.75), light.point_on(3, 1));
}

TEST(AreaLight, IntensityAt) {
  auto w = World::default_world();
  auto corner = Tuple::point(-0.5, -0.5, -5);
  auto v1 = Tuple::vector(1, 0, 0);
  auto v2 = Tuple::vector(0, 1, 0);
  auto light = AreaLight(corner, v1, 2, v2, 2, Color(1, 1, 1));

  EXPECT_EQ(0.0, light.intensity_at(Tuple::point(0, 0, 2), &w));
  EXPECT_EQ(0.25, light.intensity_at(Tuple::point(1, -1, 2), &w));
  EXPECT_EQ(0.5, light.intensity_at(Tuple::point(1.5, 0, 2), &w));
  EXPECT_EQ(0.75, light.intensity_at(Tuple::point(1.25, 1.25, 3), &w));
  EXPECT_EQ(1.0, light.intensity_at(Tuple::point(0, 0, -2), &w));
}