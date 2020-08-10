//
// Created by Brian Landers on 7/27/20.
//

#include "../shape.h"

#include "../group.h"
#include "../matrix.h"
#include "../sphere.h"
#include "gtest/gtest.h"

class TestShape : public Shape {
 public:
  TestShape()
      : Shape(),
        saved_ray_{Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 0))} {}

  Ray saved_ray() const { return saved_ray_; }

  IntersectionVector local_intersect(const Ray& r) override {
    saved_ray_ = r;
    return {};
  }

  Tuple local_normal_at(const Tuple& p) override {
    return Tuple::vector(p.x, p.y, p.z);
  }

  bool compare(const Shape&) const noexcept override { return true; }

 private:
  Ray saved_ray_;
};

TEST(ShapeTest, DefaultTransformation) {
  auto s = TestShape();
  EXPECT_EQ(Matrix(IDENTITY), s.transform());
}

TEST(ShapeTest, SetTransform) {
  auto s = TestShape();
  s.set_transform(CreateTranslation(2, 3, 4));
  EXPECT_EQ(CreateTranslation(2, 3, 4), s.transform());
}

TEST(ShapeTest, DefaultMaterial) {
  auto s = TestShape();
  auto exp = Material();
  auto m = s.material();
  EXPECT_EQ(exp, *m);
}

TEST(ShapeTest, SetMaterial) {
  auto s = TestShape();
  auto m = Material();
  m.set_ambient(1);
  s.set_material(m);
  EXPECT_EQ(m, *(s.material()));
}

TEST(ShapeTest, IntersectScaled) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = TestShape();
  s.set_transform(CreateScaling(2, 2, 2));
  auto xs = s.intersects(r);
  EXPECT_EQ(Tuple::point(0, 0, -2.5), s.saved_ray().origin());
  EXPECT_EQ(Tuple::vector(0, 0, 0.5), s.saved_ray().direction());
}

TEST(ShapeTest, IntersectTranslated) {
  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
  auto s = TestShape();
  s.set_transform(CreateTranslation(5, 0, 0));
  auto xs = s.intersects(r);
  EXPECT_EQ(Tuple::point(-5, 0, -5), s.saved_ray().origin());
  EXPECT_EQ(Tuple::vector(0, 0, 1), s.saved_ray().direction());
}

TEST(ShapeTest, NormalTranslated) {
  auto s = TestShape();
  s.set_transform(CreateTranslation(0, 1, 0));
  auto n = s.normal_at(Tuple::point(0, 1.70711, -0.70711));
  EXPECT_EQ(Tuple::vector(0, 0.70711, -0.70711), n);
}

TEST(ShapeTest, NormalTransformed) {
  auto s = TestShape();
  s.set_transform(CreateScaling(1, 0.5, 1) * CreateRotationZ(PI_5));
  auto n = s.normal_at(Tuple::point(0, SQRT2_2, -SQRT2_2));
  EXPECT_EQ(Tuple::vector(0, 0.97014, -0.24254), n);
}

TEST(Groups, Create) {
  auto g = Group();
  EXPECT_EQ(Matrix(IDENTITY), g.transform());
  EXPECT_EQ(0, g.size());
}

TEST(Groups, Parent) {
  auto s = TestShape();
  EXPECT_EQ(nullptr, s.parent());
}

TEST(Groups, AddChild) {
  auto s = std::shared_ptr<Shape>();
  s.reset(new TestShape());
  auto g = Group();
  g.add(s);
  EXPECT_NE(0, g.size());
  EXPECT_TRUE(g.contains(s));
  EXPECT_EQ(&g, s->parent());
}

TEST(Groups, IntersectRayEmpty) {
  auto g = Group();
  auto r = Ray(Tuple::point(0, 0, 0), Tuple::vector(0, 0, 1));
  auto xs = g.local_intersect(r);
  EXPECT_EQ(0, xs.size());
}

//TEST(Groups, IntersectRayNotEmpty) {
//  auto g = Group();
//
//  std::shared_ptr<Shape> s1;
//  s1.reset(new Sphere());
//
//  std::shared_ptr<Shape> s2;
//  s2.reset(new Sphere());
//  s2->set_transform(CreateTranslation(0, 0, -3));
//
//  std::shared_ptr<Shape> s3;
//  s3.reset(new Sphere());
//  s3->set_transform(CreateTranslation(5, 0, 0));
//
//  g.add(s1);
//  g.add(s2);
//  g.add(s3);
//
//  auto r = Ray(Tuple::point(0, 0, -5), Tuple::vector(0, 0, 1));
//  auto xs = g.local_intersect(r);
//
//  EXPECT_EQ(4, xs.size());
//  EXPECT_EQ(s2, xs[0].object());
//  EXPECT_EQ(s2, xs[1].object());
//  EXPECT_EQ(s1, xs[2].object());
//  EXPECT_EQ(s1, xs[3].object());
//}

//TEST(Groups, IntersectTransformed) {
//  auto g = Group();
//  g.set_transform(CreateScaling(2, 2, 2));
//
//  std::shared_ptr<Shape> s;
//  s.reset(new Sphere());
//  s->set_transform(CreateTranslation(5, 0, 0));
//  g.add(s);
//
//  auto r = Ray(Tuple::point(10, 0, -10), Tuple::vector(0, 0, 1));
//  auto xs = g.intersects(r);
//  EXPECT_EQ(2, xs.size());
//}

TEST(Groups, WorldToObject) {
  auto g1 = Group();
  g1.set_transform(CreateRotationY(PI_2));

  auto g2 = Group();
  g2.set_transform(CreateScaling(2, 2, 2));
  auto gp = std::make_shared<Group>(g2);
  g1.add(gp);

  auto s = std::shared_ptr<Shape>();
  s.reset(new Sphere());

  s->set_transform(CreateTranslation(5, 0, 0));
  gp->add(s);

  auto p = s->worldToObject(Tuple::point(-2, 0, -10));
  EXPECT_EQ(Tuple::point(0, 0, -1), p);
}

TEST(Groups, NormalFromObjectToWorld) {
  auto g1 = Group();
  g1.set_transform(CreateRotationY(PI_2));

  auto g2 = Group();
  g2.set_transform(CreateScaling(1, 2, 3));
  auto gp = std::make_shared<Group>(g2);
  g1.add(gp);

  auto s = std::shared_ptr<Shape>();
  s.reset(new Sphere());

  s->set_transform(CreateTranslation(5, 0, 0));
  gp->add(s);

  auto SQRT3_3 = sqrt(3) / 3.0;
  auto n = s->normalToWorld(Tuple::vector(SQRT3_3, SQRT3_3, SQRT3_3));
  EXPECT_EQ(Tuple::vector(0.2857, 0.4286, -0.8571), n);
}

TEST(Groups, NormalOnChild) {
  auto g1 = Group();
  g1.set_transform(CreateRotationY(PI_2));

  auto g2 = Group();
  g2.set_transform(CreateScaling(1, 2, 3));
  auto gp = std::make_shared<Group>(g2);
  g1.add(gp);

  auto s = std::shared_ptr<Shape>();
  s.reset(new Sphere());

  s->set_transform(CreateTranslation(5, 0, 0));
  gp->add(s);

  auto n = s->normal_at(Tuple::point(1.7321, 1.1547, -5.5774));
  EXPECT_EQ(Tuple::vector(0.2857, 0.4286, -0.8571), n);
}

TEST(Groups, MakeSubgroup) {
  auto s1 = std::make_shared<Sphere>();
  auto s2 = std::make_shared<Sphere>();
  auto g = Group();
  g.make_subgroup({s1, s2});
  EXPECT_EQ(1, g.size());

  auto g2 = g.child<Group>(0);
  EXPECT_NE(nullptr, g2);
  ASSERT_EQ(2, g2->size());
  EXPECT_EQ(s1, g2->child<Sphere>(0));
  EXPECT_EQ(s2, g2->child<Sphere>(1));
}

TEST(Groups, Divide) {
  auto s1 = std::make_shared<Sphere>();
  s1->set_transform(CreateTranslation(-2, -2, 0));
  auto s2 = std::make_shared<Sphere>();
  s2->set_transform(CreateTranslation(-2, 2, 0));
  auto s3 = std::make_shared<Sphere>();
  s3->set_transform(CreateScaling(4, 4, 4));
  auto g = Group();

  g.add(s1);
  g.add(s2);
  g.add(s3);
  g.divide(1);

  ASSERT_EQ(2, g.children().size());
  EXPECT_EQ(s3, g.child<Sphere>(0));
  auto subgroup = g.child<Group>(1);
  EXPECT_EQ(2, subgroup->children().size());

  ASSERT_EQ(1, subgroup->child<Group>(0)->size());
  EXPECT_EQ(s1, subgroup->child<Group>(0)->child<Sphere>(0));

  ASSERT_EQ(1, subgroup->child<Group>(1)->size());
  EXPECT_EQ(s2, subgroup->child<Group>(1)->child<Sphere>(0));
}

TEST(Groups, DivideTooFewChildren) {
  auto s1 = std::make_shared<Sphere>();
  s1->set_transform(CreateTranslation(-2, 0, 0));
  auto s2 = std::make_shared<Sphere>();
  s2->set_transform(CreateTranslation(2, 1, 0));
  auto s3 = std::make_shared<Sphere>();
  s3->set_transform(CreateTranslation(2, -1, 0));

  auto subgroup = std::make_shared<Group>();
  subgroup->add(s1);
  subgroup->add(s2);
  subgroup->add(s3);

  auto s4 = std::make_shared<Sphere>();

  auto g = std::make_shared<Group>();
  g->add(subgroup);
  g->add(s4);

  subgroup->divide(3);

  ASSERT_EQ(2, g->children().size());
  EXPECT_EQ(subgroup, g->child<Group>(0));
  EXPECT_EQ(s4, g->child<Sphere>(1));

  ASSERT_EQ(2, subgroup->size());
  EXPECT_EQ(s1, subgroup->child<Group>(0)->child<Sphere>(0));
  ASSERT_EQ(2, subgroup->child<Group>(1)->size());
  EXPECT_EQ(s2, subgroup->child<Group>(1)->child<Sphere>(0));
  EXPECT_EQ(s3, subgroup->child<Group>(1)->child<Sphere>(1));
}