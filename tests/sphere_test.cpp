//
// Created by Brian Landers on 2019-01-10.
//

#include "../sphere.h"

#include "gtest/gtest.h"
#include "../material.h"
#include "../matrix.h"
#include "../ray.h"
#include "../tuple.h"

bool vector_is_near(Tuple a, Tuple b, float abs) {
  std::cout << "A: " << a << std::endl;
  std::cout << "B: " << b << std::endl;

  return ((std::abs(a.x - b.x) < abs) && (std::abs(a.y - b.y) < abs) &&
          (std::abs(a.z - b.z) < abs) && (a.w == b.w));
}

TEST(Sphere, Intersects) {
  auto p = Tuple::point(0, 0, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(4.0, i[0].t());
  ASSERT_EQ(6.0, i[1].t());
}

TEST(Sphere, Tangent) {
  auto p = Tuple::point(0, 1, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(5.0, i[0].t());
  ASSERT_EQ(5.0, i[1].t());
}

TEST(Sphere, Misses) {
  auto p = Tuple::point(0, 2, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(0, i.size());
}

TEST(Sphere, Inside) {
  auto p = Tuple::point(0, 0, 0);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(-1.0, i[0].t());
  ASSERT_EQ(1.0, i[1].t());
}

TEST(Sphere, Behind) {
  auto p = Tuple::point(0, 0, 5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(-6.0, i[0].t());
  ASSERT_EQ(-4.0, i[1].t());
}

TEST(Sphere, Encapsulates) {
  auto p = Tuple::point(0, 0, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  Sphere s;

  auto i = s.intersects(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(&s, i[0].object());
  ASSERT_EQ(&s, i[1].object());
}

TEST(Sphere, HitPositive) {
  Sphere s;
  Intersection i1(1, &s);
  Intersection i2(2, &s);
  auto i = Hit({i2, i1});
  ASSERT_EQ(i1, i);
}

TEST(Sphere, HitSomeNegative) {
  Sphere s;
  Intersection i1(-1, &s);
  Intersection i2(1, &s);
  auto i = Hit({i2, i1});
  ASSERT_EQ(i2, i);
}

TEST(Sphere, HitAllNegative) {
  Sphere s;
  Intersection i1(-2, &s);
  Intersection i2(-1, &s);
  auto i = Hit({i2, i1});
  ASSERT_FALSE(i);
}

TEST(Sphere, HitMany) {
  Sphere s;
  Intersection i1(5, &s);
  Intersection i2(7, &s);
  Intersection i3(-3, &s);
  Intersection i4(2, &s);
  auto i = Hit({i1, i2, i3, i4});
  ASSERT_EQ(i4, i);
}

TEST(Sphere, DefaultTransform) {
  Sphere s;
  ASSERT_EQ(Matrix{IDENTITY}, s.transform());
}

TEST(Sphere, IntersectScaledWithRay) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  Sphere s;
  s.set_transform(CreateScaling(2, 2, 2));
  auto xs = s.intersects(r);
  ASSERT_EQ(2, xs.size());
  // std::cout << "xs[0]: " << xs[0] << std::endl;
  // std::cout << "xs[1]: " << xs[1] << std::endl;
  ASSERT_EQ(3, xs[0].t());
  ASSERT_EQ(7, xs[1].t());
}

TEST(Sphere, IntersectTranslatedWithRay) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  Sphere s;
  s.set_transform(CreateTranslation(5, 0, 0));
  auto xs = s.intersects(r);
  ASSERT_EQ(0, xs.size());
}

TEST(Sphere, NormalX) {
  Sphere s;
  auto n = s.normal_at(Tuple::point(1, 0, 0));
  ASSERT_EQ(Tuple::vector(1, 0, 0), n);
}

TEST(Sphere, NormalY) {
  Sphere s;
  auto n = s.normal_at(Tuple::point(0, 1, 0));
  ASSERT_EQ(Tuple::vector(0, 1, 0), n);
}

TEST(Sphere, NormalZ) {
  Sphere s;
  auto n = s.normal_at(Tuple::point(0, 0, 1));
  ASSERT_EQ(Tuple::vector(0, 0, 1), n);
}

TEST(Sphere, NormalNonAxis) {
  Sphere s;
  auto n = s.normal_at(
      Tuple::point(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0));

  ASSERT_TRUE(vector_is_near(
      Tuple::vector(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0), n,
      0.001));
}

TEST(Sphere, NormalIsNormalized) {
  Sphere s;
  auto n = s.normal_at(
      Tuple::point(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0));

  ASSERT_TRUE(vector_is_near(n.normalize(), n, 0.001));
}

TEST(Sphere, TranslatedNormal1) {
  Sphere s;
  s.set_transform(CreateTranslation(0, 1, 0));
  auto n = s.normal_at(Tuple::point(0, 1.70711, -0.70711));
  ASSERT_TRUE(vector_is_near(Tuple::vector(0, 0.70711, -0.70711), n, 0.0001));
}

TEST(Sphere, TranslatedNormal2) {
  Sphere s;
  s.set_transform(CreateScaling(1, 0.5, 1) * CreateRotationZ(PI / 5));
  auto n = s.normal_at(Tuple::point(0, sqrt(2.0) / 2, -1 * sqrt(2.0) / 2));
  ASSERT_TRUE(vector_is_near(Tuple::vector(0, 0.97014, -0.24254), n, 0.001));
}

TEST(Sphere, DefaultMaterial) {
  auto s = Sphere();
  auto exp = Material();
  auto m = s.material();
  EXPECT_EQ(exp, *m);
}

TEST(Sphere, SetMaterial) {
  auto s = Sphere();
  auto m = Material();
  m.set_ambient(1);
  s.set_material(m);
  EXPECT_EQ(m, *(s.material()));
}

TEST(Sphere, PreComp) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto shape = Sphere();
  auto i = Intersection(4, &shape);

  auto comps = ComputedIntersection(i, r);
  EXPECT_EQ(i.object(), comps.object);
  EXPECT_EQ(Tuple::point(0, 0, -1), comps.point);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.eyev);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.normalv);
}

TEST(Sphere, PreCompOutside) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto shape = Sphere();
  auto i = Intersection(4, &shape);

  auto comps = ComputedIntersection(i, r);
  EXPECT_FALSE(comps.inside);
}

TEST(Sphere, PreCompInside) {
  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto shape = Sphere();
  auto i = Intersection(1, &shape);

  auto comps = ComputedIntersection(i, r);
  EXPECT_TRUE(comps.inside);
  EXPECT_EQ(Tuple::point(0, 0, 1), comps.point);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.eyev);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.normalv);
}