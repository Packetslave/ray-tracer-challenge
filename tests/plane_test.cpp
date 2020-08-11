//
// Created by Brian Landers on 7/27/20.
//

#include "../shapes/plane.h"

#include "../shapes/shape.h"
#include "gtest/gtest.h"

TEST(PlaneTest, NormalIsConstant) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto n1 = plane->local_normal_at(Tuple::point(0, 0, 0), nullptr);
  auto n2 = plane->local_normal_at(Tuple::point(10, 0, -10), nullptr);
  auto n3 = plane->local_normal_at(Tuple::point(-5, 0, 150), nullptr);
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
  ASSERT_EQ(plane.get(), xs[0].object());
}

TEST(PlaneTest, IntersectBelow) {
  std::shared_ptr<Shape> plane;
  plane.reset(new Plane());

  auto r = Ray(Tuple::point(0, -1, 0), Tuple::vector(0, 1, 0));
  auto xs = plane->local_intersect(r);
  ASSERT_EQ(1, xs.size());
  ASSERT_EQ(1, xs[0].t());
  ASSERT_EQ(plane.get(), xs[0].object());
}

TEST(PlaneTest, PrecomputeReflection) {
  std::shared_ptr<Shape> s;
  s.reset(new Plane());

  auto r = Ray(Tuple::point(0, 1, -1), Tuple::vector(0, -SQRT2_2, SQRT2_2));
  auto i = Intersection(SQRT2_2, s.get());
  auto comps = ComputedIntersection(i, r);
  EXPECT_EQ(Tuple::vector(0, SQRT2_2, SQRT2_2), comps.reflectv);
}

TEST(PlaneTest, DISABLED_BoundingBox) {
  auto s = Plane();
  auto box = s.bounds_of();
  EXPECT_EQ(Tuple::point(-DBL_MAX, 0, -DBL_MAX), box->min());
  EXPECT_EQ(Tuple::point(DBL_MAX, 0, DBL_MAX), box->max());
}