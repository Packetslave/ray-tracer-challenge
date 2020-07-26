//
// Created by Brian Landers on 2019-01-10.
//

#include "../ray.h"

#include "../matrix.h"
#include "../tuple.h"
#include "gtest/gtest.h"

TEST(Ray, Create) {
  auto p = Tuple::point(1, 2, 3);
  auto v = Tuple::vector(4, 5, 6);

  Ray r(p, v);
  ASSERT_EQ(p, r.origin());
  ASSERT_EQ(v, r.direction());
}

TEST(Ray, ComputePointFromDistance) {
  auto p = Tuple::point(2, 3, 4);
  auto v = Tuple::vector(1, 0, 0);

  Ray r(p, v);
  ASSERT_EQ(Tuple::point(2, 3, 4), r.position(0));
  ASSERT_EQ(Tuple::point(3, 3, 4), r.position(1));
  ASSERT_EQ(Tuple::point(1, 3, 4), r.position(-1));
  ASSERT_EQ(Tuple::point(4.5, 3, 4), r.position(2.5));
}

TEST(Ray, TranslateRay) {
  auto r = Ray(Tuple::point(1, 2, 3), Tuple::vector(0, 1, 0));
  auto m = CreateTranslation(3, 4, 5);
  auto r2 = r.transform(m);
  ASSERT_EQ(Tuple::point(4, 6, 8), r2.origin());
  ASSERT_EQ(Tuple::vector(0, 1, 0), r2.direction());
}

TEST(Ray, ScaleRay) {
  auto r = Ray(Tuple::point(1, 2, 3), Tuple::vector(0, 1, 0));
  auto m = CreateScaling(2, 3, 4);
  auto r2 = r.transform(m);
  ASSERT_EQ(Tuple::point(2, 6, 12), r2.origin());
  ASSERT_EQ(Tuple::vector(0, 3, 0), r2.direction());
}
