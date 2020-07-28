//
// Created by Brian Landers on 2019-01-10.
//

#include "../sphere.h"

#include "../material.h"
#include "../matrix.h"
#include "../ray.h"
#include "../tuple.h"
#include "gtest/gtest.h"

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
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(4.0, i[0].t());
  ASSERT_EQ(6.0, i[1].t());
}

TEST(Sphere, Tangent) {
  auto p = Tuple::point(0, 1, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(5.0, i[0].t());
  ASSERT_EQ(5.0, i[1].t());
}

TEST(Sphere, Misses) {
  auto p = Tuple::point(0, 2, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(0, i.size());
}

TEST(Sphere, Inside) {
  auto p = Tuple::point(0, 0, 0);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(-1.0, i[0].t());
  ASSERT_EQ(1.0, i[1].t());
}

TEST(Sphere, Behind) {
  auto p = Tuple::point(0, 0, 5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(-6.0, i[0].t());
  ASSERT_EQ(-4.0, i[1].t());
}

TEST(Sphere, Encapsulates) {
  auto p = Tuple::point(0, 0, -5);
  auto v = Tuple::vector(0, 0, 1);
  Ray r(p, v);
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  auto i = s->local_intersect(r);
  ASSERT_EQ(2, i.size());
  ASSERT_EQ(s, i[0].object());
  ASSERT_EQ(s, i[1].object());
}

TEST(Sphere, HitPositive) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());

  Intersection i1(1, s);
  Intersection i2(2, s);
  auto i = Hit({i2, i1});
  ASSERT_EQ(i1, i.value());
}

TEST(Sphere, HitSomeNegative) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  Intersection i1(-1, s);
  Intersection i2(1, s);
  auto i = Hit({i2, i1});
  ASSERT_EQ(i2, i.value());
}

TEST(Sphere, HitAllNegative) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  Intersection i1(-2, s);
  Intersection i2(-1, s);
  auto i = Hit({i2, i1});
  ASSERT_FALSE(i);
}

TEST(Sphere, HitMany) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  Intersection i1(5, s);
  Intersection i2(7, s);
  Intersection i3(-3, s);
  Intersection i4(2, s);
  auto i = Hit({i1, i2, i3, i4});
  ASSERT_EQ(i4, i.value());
}

TEST(Sphere, DefaultTransform) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  ASSERT_EQ(Matrix{IDENTITY}, s->transform());
}

TEST(Sphere, NormalX) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto n = s->local_normal_at(Tuple::point(1, 0, 0));
  ASSERT_EQ(Tuple::vector(1, 0, 0), n);
}

TEST(Sphere, NormalY) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto n = s->local_normal_at(Tuple::point(0, 1, 0));
  ASSERT_EQ(Tuple::vector(0, 1, 0), n);
}

TEST(Sphere, NormalZ) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto n = s->local_normal_at(Tuple::point(0, 0, 1));
  ASSERT_EQ(Tuple::vector(0, 0, 1), n);
}

TEST(Sphere, NormalNonAxis) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto n = s->normal_at(
      Tuple::point(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0));

  ASSERT_TRUE(vector_is_near(
      Tuple::vector(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0), n,
      0.001));
}

TEST(Sphere, NormalIsNormalized) {
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto n = s->normal_at(
      Tuple::point(sqrt(3.0) / 3.0, sqrt(3.0) / 3.0, sqrt(3.0) / 3.0));

  ASSERT_TRUE(vector_is_near(n.normalize(), n, 0.001));
}

TEST(Sphere, PreComp) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto i = Intersection(4, s);

  auto comps = ComputedIntersection(i, r);
  EXPECT_EQ(i.object(), comps.object);
  EXPECT_EQ(Tuple::point(0, 0, -1), comps.point);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.eyev);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.normalv);
}

TEST(Sphere, PreCompOutside) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto i = Intersection(4, s);

  auto comps = ComputedIntersection(i, r);
  EXPECT_FALSE(comps.inside);
}

TEST(Sphere, PreCompInside) {
  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  std::shared_ptr<Shape> s;
  s.reset(new Sphere());
  auto i = Intersection(1, s);

  auto comps = ComputedIntersection(i, r);
  EXPECT_TRUE(comps.inside);
  EXPECT_EQ(Tuple::point(0, 0, 1), comps.point);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.eyev);
  EXPECT_EQ(Tuple::vector(0, 0, -1), comps.normalv);
}

TEST(Sphere, GlassSphere) {
  auto s = Sphere::Glass();
  EXPECT_EQ(Matrix(IDENTITY), s->transform());
  EXPECT_EQ(1.0, s->material()->transparency());
  EXPECT_EQ(1.5, s->material()->refractive());
}

TEST(Sphere, FindingN1andN2) {
  auto a = Sphere::Glass();
  a->set_transform(CreateScaling(2, 2, 2));
  a->material()->set_refractive(1.5);

  auto b = Sphere::Glass();
  b->set_transform(CreateTranslation(0, 0, -0.25));
  b->material()->set_refractive(2.0);

  auto c = Sphere::Glass();
  c->set_transform(CreateTranslation(0, 0, 0.25));
  c->material()->set_refractive(2.5);

  auto r = Ray(Tuple::point(0, 0, -4), Tuple::vector(0, 0, 1));
  auto xs = std::vector<Intersection> {
    {
      Intersection(2, a), Intersection(2.75, b), Intersection(3.25, c),
          Intersection(4.75, b), Intersection(5.25, c), Intersection(6, a),
    }
  };
  // HERE
}