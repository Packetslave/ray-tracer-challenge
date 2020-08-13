#include "../core/world.h"

#include <cmath>

#include "../core/camera.h"
#include "../core/light.h"
#include "../core/material.h"
#include "../core/tuple.h"
#include "../shapes/plane.h"
#include "../shapes/sphere.h"
#include "gtest/gtest.h"
#include "test_common.h"

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

TEST(World, HitIntersectionOutside) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto shape = std::shared_ptr<Shape>();
  shape.reset(new Sphere());

  auto i = Intersection(4, shape.get());
  auto comps = ComputedIntersection(i, r);
  ASSERT_FALSE(comps.inside);
}

TEST(World, HitIntersectionInside) {
  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto shape = std::shared_ptr<Shape>();
  shape.reset(new Sphere());

  auto i = Intersection(1, shape.get());
  auto comps = ComputedIntersection(i, r);

  EXPECT_EQ(Tuple::point(0, 0, 1), comps.point);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.eyev);
  EXPECT_TRUE(comps.inside);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.normalv);
}

TEST(World, ShadeIntersectionOutside) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto shape = w.get_object(0);

  auto i = Intersection(4, shape.get());
  auto comps = ComputedIntersection(i, r);
  auto c = w.shade_hit(comps);
  EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), c);
}

TEST(World, ShadeIntersectionInside) {
  auto w = World::default_world();
  w.set_light(PointLight(Tuple::point(0, 0.25, 0), Color(1, 1, 1)));

  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto shape = w.get_object(1);

  auto i = Intersection(0.5, shape.get());
  auto comps = ComputedIntersection(i, r);
  auto c = w.shade_hit(comps);
  EXPECT_EQ(Color(0.90498, 0.90498, 0.90498), c);
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

  auto s1 = std::make_shared<Sphere>();
  w.add(s1);

  auto s2 = std::make_shared<Sphere>();
  s2->set_transform(CreateTranslation(0, 0, 10));
  w.add(s2);

  auto r = Ray(Tuple::point(0, 0, 5), Tuple::vector(0, 0, 1));
  auto i = Intersection(4, s2.get());

  auto comps = ComputedIntersection(i, r);
  auto c = w.shade_hit(comps);

  EXPECT_EQ(Color(0.1, 0.1, 0.1), c);
}

TEST(Shadows, HitOffset) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = std::make_shared<Sphere>();
  s->set_transform((CreateTranslation(0, 0, 1)));
  auto i = Intersection(5, s.get());
  auto comps = ComputedIntersection(i, r);
  EXPECT_TRUE(comps.over_point.z < -EPSILON / 2);
  EXPECT_TRUE(comps.point.z > comps.over_point.z);
}

TEST(Reflect, NonReflectiveMaterial) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto s = w.get_object(1);
  s->material()->set_ambient(1);
  auto i = Intersection(1, s.get());
  auto comps = ComputedIntersection(i, r);
  auto color = w.reflected_color(comps);
  EXPECT_EQ(Color(0, 0, 0), color);
}

TEST(Reflect, ReflectiveMaterial) {
  auto w = World::default_world();
  std::shared_ptr<Shape> s;
  s.reset(new Plane());

  auto m = Material();
  m.set_reflective(0.5);
  s->set_material(m);
  s->set_transform(CreateTranslation(0, -1, 0));
  w.add(s);

  auto r = Ray(Tuple::point(0, 0, -3), Tuple::vector(0, -SQRT2_2, SQRT2_2));
  auto i = Intersection(sqrt(2), s.get());
  auto comps = ComputedIntersection(i, r);
  auto color = w.reflected_color(comps);
  EXPECT_EQ(Color(0.19032, 0.2379, 0.14274), color);
}

TEST(Reflect, ShadeHit) {
  auto w = World::default_world();

  std::shared_ptr<Shape> shape;
  shape.reset(new Plane());
  shape->material()->set_reflective(0.5);
  shape->set_transform(CreateTranslation(0, -1, 0));
  w.add(shape);

  auto r = Ray(Tuple::point(0, 0, -3), Tuple::vector(0, -SQRT2_2, SQRT2_2));
  auto i = Intersection(sqrt(2), shape.get());
  auto comps = ComputedIntersection(i, r);
  auto color = w.shade_hit(comps);
  EXPECT_EQ(Color(0.87677, 0.92436, 0.82918), color);
}

TEST(Reflect, ColorAtMutuallyReflective) {
  auto w = World();
  w.set_light(PointLight(Tuple::point(0, 0, 0), Color(1, 1, 1)));

  std::shared_ptr<Shape> lower;
  lower.reset(new Plane());
  lower->material()->set_reflective(1);
  lower->set_transform(CreateTranslation(0, -1, 0));
  w.add(lower);

  std::shared_ptr<Shape> upper;
  upper.reset(new Plane());
  upper->material()->set_reflective(1);
  upper->set_transform(CreateTranslation(0, 1, 0));
  w.add(upper);

  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 1, 0));
  auto c = w.color_at(r);
}

TEST(Reflect, ColorAtMaxRecursion) {
  auto w = World();
  w.set_light(PointLight(Tuple::point(0, 0, 0), Color(1, 1, 1)));

  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());
  plane->material()->set_reflective(0.5);
  plane->set_transform(CreateTranslation(0, -1, 0));
  w.add(plane);

  auto r = Ray(Tuple::point(0, 0, -3), Tuple::vector(0, -SQRT2_2, SQRT2_2));
  auto i = Intersection(sqrt(2), plane.get());
  auto comps = ComputedIntersection(i, r);
  auto color = w.reflected_color(comps, 0);
  EXPECT_EQ(Color(0, 0, 0), color);
}

TEST(Refract, RefractedColorOpaqueSurface) {
  auto w = World::default_world();
  auto shape = w.get_object(0);
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto xs = IntersectionVector{Intersection(4, shape.get()),
                               Intersection(5, shape.get())};
  auto comps = ComputedIntersection(xs[0], r, xs);
  auto c = w.refracted_color(comps, 5);
  EXPECT_EQ(Color(0, 0, 0), c);
}

TEST(Refract, RefractedColorMaxDepth) {
  auto w = World::default_world();
  auto shape = w.get_object(0);
  shape->material()->set_transparency(1.0);
  shape->material()->set_refractive(1.5);

  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto xs = IntersectionVector{Intersection(4, shape.get()),
                               Intersection(5, shape.get())};

  auto comps = ComputedIntersection(xs[0], r, xs);
  auto c = w.refracted_color(comps, 0);
  EXPECT_EQ(Color(0, 0, 0), c);
}

TEST(Refract, RefractedColorTotalInternal) {
  auto w = World::default_world();
  auto shape = w.get_object(0);
  shape->material()->set_transparency(1.0);
  shape->material()->set_refractive(1.5);

  auto r = Ray(Tuple::point(0, 0, SQRT2_2), Tuple::vector(0, 1, 0));
  auto xs = IntersectionVector{Intersection(-SQRT2_2, shape.get()),
                               Intersection(SQRT2_2, shape.get())};

  auto comps = ComputedIntersection(xs[1], r, xs);
  auto c = w.refracted_color(comps, 5);
  EXPECT_EQ(Color(0, 0, 0), c);
}

TEST(Refract, RefractedColor) {
  auto w = World::default_world();
  auto a = w.get_object(0);
  a->material()->set_ambient(1.0);

  auto p = TestPattern();
  a->material()->set_pattern(p);

  auto b = w.get_object(1);
  b->material()->set_transparency(1.0);
  b->material()->set_refractive(1.5);

  auto r = Ray(Tuple::point(0, 0, 0.1), Tuple::vector(0, 1, 0));
  auto xs = IntersectionVector{
      Intersection(-0.9899, a.get()), Intersection(-0.4899, b.get()),
      Intersection(0.4899, b.get()), Intersection(0.9899, a.get())};

  auto comps = ComputedIntersection(xs[2], r, xs);
  auto c = w.refracted_color(comps, 5);
  EXPECT_EQ(Color(0, 0.99888, 0.04725), c);
}

TEST(Refract, ShadeHit) {
  auto w = World::default_world();
  std::shared_ptr<Shape> floor;
  floor.reset(new Plane());

  floor->set_transform(CreateTranslation(0, -1, 0));
  Material m;
  m.set_transparency(0.5);
  m.set_refractive(1.5);
  floor->set_material(m);
  w.add(floor);

  std::shared_ptr<Shape> ball;
  ball.reset(new Sphere());

  ball->set_transform(CreateTranslation(0, -3.5, -0.5));
  auto m2 = Material();
  m2.set_color(Color(1, 0, 0));
  m2.set_ambient(0.5);
  ball->set_material(m2);
  w.add(ball);
  auto r = Ray(Tuple::point(0, 0, -3), Tuple::vector(0, -SQRT2_2, SQRT2_2));
  auto xs = IntersectionVector{Intersection(sqrt(2), floor.get())};
  auto comps = ComputedIntersection(xs[0], r, xs);
  auto color = w.shade_hit(comps, 5);
  EXPECT_EQ(Color(0.93642, 0.68642, 0.68642), color);
}

TEST(Refract, ShadeHitWIthReflectiveAndTransParent) {
  auto w = World::default_world();
  auto r = Ray(Tuple::point(0, 0, -3), Tuple::vector(0, -SQRT2_2, SQRT2_2));

  std::shared_ptr<Shape> floor;
  floor.reset(new Plane());

  floor->set_transform(CreateTranslation(0, -1, 0));
  Material m;
  m.set_reflective(0.5);
  m.set_transparency(0.5);
  m.set_refractive(1.5);
  floor->set_material(m);
  w.add(floor);

  std::shared_ptr<Shape> ball;
  ball.reset(new Sphere());

  ball->set_transform(CreateTranslation(0, -3.5, -0.5));
  auto m2 = Material();
  m2.set_color(Color(1, 0, 0));
  m2.set_ambient(0.5);
  ball->set_material(m2);
  w.add(ball);

  auto xs = IntersectionVector{Intersection(sqrt(2), floor.get())};
  auto comps = ComputedIntersection(xs[0], r, xs);
  auto color = w.shade_hit(comps, 5);
  EXPECT_EQ(Color(0.93391, 0.69643, 0.69243), color);
}