//
// Created by Brian Landers on 8/8/20.
//

#include "../shapes/triangle.h"

#include "gtest/gtest.h"

TEST(Triangle, Create) {
  auto p1 = Tuple::point(0, 1, 0);
  auto p2 = Tuple::point(-1, 0, 0);
  auto p3 = Tuple::point(1, 0, 0);
  auto t = Triangle(p1, p2, p3);
  EXPECT_EQ(p1, t.p1);
  EXPECT_EQ(p2, t.p2);
  EXPECT_EQ(p3, t.p3);
  EXPECT_EQ(Tuple::vector(-1, -1, 0), t.e1);
  EXPECT_EQ(Tuple::vector(1, -1, 0), t.e2);
  EXPECT_EQ(Tuple::vector(0, 0, -1), t.normal);
}

TEST(Triangle, Normal) {
  auto p1 = Tuple::point(0, 1, 0);
  auto p2 = Tuple::point(-1, 0, 0);
  auto p3 = Tuple::point(1, 0, 0);
  auto t = Triangle(p1, p2, p3);
  auto n1 = t.local_normal_at(Tuple::point(0, 0.5, 0), nullptr);
  auto n2 = t.local_normal_at(Tuple::point(-0.5, 0.75, 0), nullptr);
  auto n3 = t.local_normal_at(Tuple::point(0.5, 0.25, 0), nullptr);
  EXPECT_EQ(t.normal, n1);
  EXPECT_EQ(t.normal, n2);
  EXPECT_EQ(t.normal, n3);
}

TEST(Triangle, IntersectParallel) {
  auto t = Triangle(Tuple::point(0, 1, 0), Tuple::point(-1, 0, 0), Tuple::point(1, 0, 0));
  auto r = Ray(Tuple::point(0, -1, -2), Tuple::vector(0, 1, 0));
  auto xs = t.local_intersect(r);
  EXPECT_EQ(0, xs.size());
}

TEST(Triangle, MissesP1P3) {
  auto t = Triangle(Tuple::point(0, 1, 0), Tuple::point(-1, 0, 0), Tuple::point(1, 0, 0));
  auto r = Ray(Tuple::point(1, 1, -2), Tuple::vector(0, 0, 1));
  auto xs = t.local_intersect(r);
  EXPECT_EQ(0, xs.size());
}

TEST(Triangle, MissesP1P2) {
  auto t = Triangle(Tuple::point(0, 1, 0), Tuple::point(-1, 0, 0), Tuple::point(1, 0, 0));
  auto r = Ray(Tuple::point(-1, 1, -2), Tuple::vector(0, 0, 1));
  auto xs = t.local_intersect(r);
  EXPECT_EQ(0, xs.size());
}

TEST(Triangle, MissesP2P3) {
  auto t = Triangle(Tuple::point(0, 1, 0), Tuple::point(-1, 0, 0), Tuple::point(1, 0, 0));
  auto r = Ray(Tuple::point(0, -1, -2), Tuple::vector(0, 0, 1));
  auto xs = t.local_intersect(r);
  EXPECT_EQ(0, xs.size());
}

TEST(Triangle, Hits) {
  std::shared_ptr<Shape> t;
  t.reset( new Triangle(Tuple::point(0, 1, 0), Tuple::point(-1, 0, 0), Tuple::point(1, 0, 0)));
  auto r = Ray(Tuple::point(0, 0.5, -2), Tuple::vector(0, 0, 1));
  auto xs = t->local_intersect(r);
  EXPECT_EQ(1, xs.size());
  EXPECT_EQ(2, xs[0].t());
}

TEST(Triangle, BoundingBox) {
  auto p1 = Tuple::point(-3, 7, 2);
  auto p2 = Tuple::point(6, 2, -4);
  auto p3 = Tuple::point(2, -1, -1);
  auto shape = Triangle(p1, p2, p3);
  auto box = shape.bounds_of();
  EXPECT_EQ(Tuple::point(-3, -1, -4), box->min());
  EXPECT_EQ(Tuple::point(6, 7, 2), box->max());
}

TEST(SmoothTriangle, Normal) {
  auto p1 = Tuple::point(0, 1, 0);
  auto p2 = Tuple::point(-1, 0, 0);
  auto p3 = Tuple::point(1, 0, 0);
  auto n1 = Tuple::vector(0, 1, 0);
  auto n2 = Tuple::vector(-1, 0, 0);
  auto n3 = Tuple::vector(1, 0, 0);
  auto tri = SmoothTriangle(p1, p2, p3, n1, n2, n3);

  auto i = Intersection(1, &tri, 0.45, 0.25);
  auto n = tri.normal_at(Tuple::point(0, 0, 0), &i);
  EXPECT_EQ(Tuple::vector(-0.5547, 0.83205, 0), n);
}

TEST(SmoothTriangle, PrepareNormal) { auto p1 = Tuple::point(0, 1, 0);
  auto p2 = Tuple::point(-1, 0, 0);
  auto p3 = Tuple::point(1, 0, 0);
  auto n1 = Tuple::vector(0, 1, 0);
  auto n2 = Tuple::vector(-1, 0, 0);
  auto n3 = Tuple::vector(1, 0, 0);
  auto tri = SmoothTriangle(p1, p2, p3, n1, n2, n3);

  auto i = Intersection(1, &tri, 0.45, 0.25);
  auto r = Ray(Tuple::point(-0.2, 0.3, -2), Tuple::vector(0, 0, 1));
  auto xs = IntersectionVector({i});
  auto comps = ComputedIntersection(i, r, xs);
  EXPECT_EQ(Tuple::vector(-0.5547, 0.83205, 0), comps.normalv);
}