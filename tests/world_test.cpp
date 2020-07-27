#include "../world.h"

#include <cmath>

#include "../camera.h"
#include "../light.h"
#include "../material.h"
#include "../sphere.h"
#include "../tuple.h"
#include "gtest/gtest.h"

bool matrix_is_near(Matrix a, Matrix b, float abs) {
  for (int row = 0; row < a.size(); ++row) {
    for (int col = 0; col < a.size(); ++col) {
      if (std::abs(a.get(row, col) - b.get(row, col)) > abs) {
        return false;
      }
    }
  }
  return true;
}

bool tuple_is_near(Tuple a, Tuple b) {
  return abs(a.x - b.x) < EPSILON && abs(a.y - b.y) < EPSILON &&
         abs(a.x - b.x) < EPSILON && abs(a.w - b.w) < EPSILON;
}

TEST(World, Create) {
  auto w = World();
  EXPECT_EQ(0, w.size());
  EXPECT_FALSE(w.light().has_value());
}

TEST(World, Default) {
  auto light = PointLight(Tuple::point(-10, 10, -10), Color(1, 1, 1));
  auto s1 = Sphere();

  auto m = Material();
  m.set_color(Color(0.8, 1.0, 0.6));
  m.set_diffuse(0.7);
  m.set_specular(0.2);
  s1.set_material(m);

  auto s2 = Sphere();
  s2.set_transform(CreateScaling(0.5, 0.5, 0.5));

  auto w = World::default_world();
  EXPECT_EQ(light, w.light());

  EXPECT_TRUE(w.contains(s1));
  EXPECT_TRUE(w.contains(s2));
}

TEST(World, IntersectRay) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto xs = w.intersect(r);
  EXPECT_EQ(4, xs.size());
  EXPECT_EQ(4, xs[0].t());
  EXPECT_EQ(4.5, xs[1].t());
  EXPECT_EQ(5.5, xs[2].t());
  EXPECT_EQ(6, xs[3].t());
}

TEST(World, ShadeIntersection) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto shape = w.get_object(0);

  auto i = Intersection(4, shape);
  auto comps = ComputedIntersection(i, r);

  auto c = w.shade_hit(comps);
  ASSERT_EQ(Color(0.38066, 0.47583, 0.2855), c);
}

TEST(World, ShadeIntersectionInside) {
  auto w = World::default_world();
  w.set_light(PointLight(Tuple::point(0, 0.25, 0), Color(1, 1, 1)));

  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto shape = w.get_object(1);

  auto i = Intersection(0.5, shape);
  auto comps = ComputedIntersection(i, r);

  auto c = w.shade_hit(comps);
  ASSERT_EQ(Color(0.90498, 0.90498, 0.90498), c);
}

TEST(World, ColorAtMiss) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 1, 0));
  auto c = w.color_at(r);
  EXPECT_EQ(Color(0, 0, 0), c);
}

TEST(World, ColorAtHit) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto c = w.color_at(r);

  EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), c);
}

TEST(World, ColorAtBehind) {
  auto w = World::default_world();

  auto outer = w.get_object(0);
  outer->material()->set_ambient(1);

  auto inner = w.get_object(1);
  inner->material()->set_ambient(1);

  auto r = Ray(Tuple::point(0, 0, 0.75), Tuple::vector(0, 0, -1));
  auto c = w.color_at(r);

  EXPECT_EQ(inner->material()->color(), c);
}

TEST(ViewTransform, Default) {
  auto from = Tuple::point(0, 0, 0);
  auto to = Tuple::point(0, 0, -1);
  auto up = Tuple::vector(0, 1, 0);
  auto t = view_transform(from, to, up);
  EXPECT_EQ(Matrix(IDENTITY), t);
}

TEST(ViewTransformP, PositiveZ) {
  auto from = Tuple::point(0, 0, 0);
  auto to = Tuple::point(0, 0, 1);
  auto up = Tuple::vector(0, 1, 0);
  auto t = view_transform(from, to, up);
  EXPECT_EQ(CreateScaling(-1, 1, -1), t);
}

TEST(ViewTransformP, MoveTheWorld) {
  auto from = Tuple::point(0, 0, 8);
  auto to = Tuple::point(0, 0, 0);
  auto up = Tuple::vector(0, 1, 0);
  auto t = view_transform(from, to, up);
  EXPECT_EQ(CreateTranslation(0, 0, -8), t);
}

TEST(ViewTransformP, Arbitraty) {
  auto from = Tuple::point(1, 3, 2);
  auto to = Tuple::point(4, -2, 8);
  auto up = Tuple::vector(1, 1, 0);
  auto t = view_transform(from, to, up);

  // clang-format off
  auto expected = Matrix{MatrixData4{
      {
        {-0.50709 , 0.50709 ,  0.67612 , -2.36643 },
        { 0.76772 , 0.60609 ,  0.12122 , -2.82843 },
        {-0.35857 , 0.59761 , -0.71714 ,  0.00000 },
        { 0.00000 , 0.00000 ,  0.00000 ,  1.00000 }
      }
  }};
  // clang-format on

  EXPECT_TRUE(matrix_is_near(expected, t, 0.0001));
}

TEST(Shadows, NoShadowWhenNoCollinear) {
  auto w = World::default_world();
  auto p = Tuple::point(0, 10, 0);
  EXPECT_FALSE(w.is_shadowed(p));
}

TEST(Shadows, ShadowWhenObjectIsBetweenPointAndLight) {
  auto w = World::default_world();
  auto p = Tuple::point(10, -10, 10);
  EXPECT_TRUE(w.is_shadowed(p));
}

TEST(Shadows, NoShadowWhenObjectBehindLight) {
  auto w = World::default_world();
  auto p = Tuple::point(-20, 20, -20);
  EXPECT_FALSE(w.is_shadowed(p));
}

TEST(Shadows, NoShadowWhenObjectBehindPoint) {
  auto w = World::default_world();
  auto p = Tuple::point(-2, 2, -2);
  EXPECT_FALSE(w.is_shadowed(p));
}

TEST(Shadows, ShadeHitGivenShadow) {
  auto w = World();
  w.set_light(PointLight(Tuple::point(0, 0, -10), Color(1, 1, 1)));

  auto s1 = Sphere();
  w.add(s1);

  auto s2 = Sphere();
  s2.set_transform(CreateTranslation(0, 0, 10));
  w.add(s2);

  auto r = Ray(Tuple::point(0, 0, 5), Tuple::vector(0, 0, 1));
  auto i = Intersection(4, &s2);

  auto comps = ComputedIntersection(i, r);
  auto c = w.shade_hit(comps);

  EXPECT_EQ(Color(0.1, 0.1, 0.1), c);
}

TEST(Shadows, HitOffset) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = Sphere();
  s.set_transform((CreateTranslation(0, 0, 1)));
  auto i = Intersection(5, &s);
  auto comps = ComputedIntersection(i, r);
  EXPECT_TRUE(comps.over_point.z < -EPSILON/2);
  EXPECT_TRUE(comps.point.z > comps.over_point.z);
}
