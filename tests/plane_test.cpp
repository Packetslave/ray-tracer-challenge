//
// Created by Brian Landers on 7/27/20.
//

#include "../plane.h"
#include "../shape.h"

#include "gtest/gtest.h"

TEST(PlaneTest, NormalIsConstant) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto n1 = plane->local_normal_at(Tuple::point(0, 0, 0));
  auto n2 = plane->local_normal_at(Tuple::point(10, 0, -10));
  auto n3 = plane->local_normal_at(Tuple::point(-5, 0, 150));
  EXPECT_EQ(Tuple::vector(0, 1, 0), n1);
  EXPECT_EQ(Tuple::vector(0, 1, 0), n2);
  EXPECT_EQ(Tuple::vector(0, 1, 0), n3);
}

TEST(PlaneTest, IntersectParallel) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto r = Ray(Tuple::point(0, 10, 0), Tuple::vector(0, 0, 1));
  auto xs = plane->local_intersect(r);
  ASSERT_EQ(0, xs.size());
}

TEST(PlaneTest, IntersectCoplanar) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto xs = plane->local_intersect(r);
  ASSERT_EQ(0, xs.size());
}

TEST(PlaneTest, IntersectAbove) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto r = Ray(Tuple::point(0, 1, 0), Tuple::vector(0, -1, 0));
  auto xs = plane->local_intersect(r);
  ASSERT_EQ(1, xs.size());
  ASSERT_EQ(1, xs[0].t());
  ASSERT_EQ(plane, xs[0].object());
}

TEST(PlaneTest, IntersectBelow) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto r = Ray(Tuple::point(0, -1, 0), Tuple::vector(0, 1, 0));
  auto xs = plane->local_intersect(r);
  ASSERT_EQ(1, xs.size());
  ASSERT_EQ(1, xs[0].t());
  ASSERT_EQ(plane, xs[0].object());
}