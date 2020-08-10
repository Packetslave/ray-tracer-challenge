#include "../core/bounding_box.h"

#include "../core/tuple.h"
#include "../shapes/group.h"
#include "../shapes/sphere.h"
#include "gtest/gtest.h"
#include "test_common.h"

TEST(BoundingBox, CreateEmpty) {
  auto box = BoundingBox();
  EXPECT_EQ(POSITIVE_INF, box.min());
  EXPECT_EQ(NEGATIVE_INF, box.max());
}

TEST(BoundingBox, CreateWithVolume) {
  auto box = BoundingBox(Tuple::point(-1, -2, -3), Tuple::point(3, 2, 1));
  EXPECT_EQ(Tuple::point(-1, -2, -3), box.min());
  EXPECT_EQ(Tuple::point(3, 2, 1), box.max());
}

TEST(BoundingBox, AddToEmpty) {
  auto box = BoundingBox();
  auto p1 = Tuple::point(-5, 2, 0);
  auto p2 = Tuple::point(7, 0, -3);
  box.add(p1);
  box.add(p2);
  EXPECT_EQ(Tuple::point(-5, 0, -3), box.min());
  EXPECT_EQ(Tuple::point(7, 2, 0), box.max());
}

TEST(BoundingBox, Split) {
  auto b = BoundingBox(Tuple::point(-1, -4, -5), Tuple::point(9, 6, 5));
  auto [left, right] = b.split();
  EXPECT_EQ(Tuple::point(-1, -4, -5), left.min());
  EXPECT_EQ(Tuple::point(4, 6, 5), left.max());
  EXPECT_EQ(Tuple::point(4, -4, -5), right.min());
  EXPECT_EQ(Tuple::point(9, 6, 5), right.max());
}

TEST(BoundingBox, SplitXwide) {
  auto b = BoundingBox(Tuple::point(-1, -2, -3), Tuple::point(9, 5.5, 3));
  auto [left, right] = b.split();
  EXPECT_EQ(Tuple::point(-1, -2, -3), left.min());
  EXPECT_EQ(Tuple::point(4, 5.5, 3), left.max());
  EXPECT_EQ(Tuple::point(4, -2, -3), right.min());
  EXPECT_EQ(Tuple::point(9, 5.5, 3), right.max());
}

TEST(BoundingBox, SplitYwide) {
  auto b = BoundingBox(Tuple::point(-1, -2, -3), Tuple::point(5, 8, 3));
  auto [left, right] = b.split();
  EXPECT_EQ(Tuple::point(-1, -2, -3), left.min());
  EXPECT_EQ(Tuple::point(5, 3, 3), left.max());
  EXPECT_EQ(Tuple::point(-1, 3, -3), right.min());
  EXPECT_EQ(Tuple::point(5, 8, 3), right.max());
}

TEST(BoundingBox, SplitZwide) {
  auto b = BoundingBox(Tuple::point(-1, -2, -3), Tuple::point(5, 3, 7));
  auto [left, right] = b.split();
  EXPECT_EQ(Tuple::point(-1, -2, -3), left.min());
  EXPECT_EQ(Tuple::point(5, 3, 2), left.max());
  EXPECT_EQ(Tuple::point(-1, -2, 2), right.min());
  EXPECT_EQ(Tuple::point(5, 3, 7), right.max());
}

TEST(BoundingBox, PartitionChildren) {
  auto s1 = std::make_shared<Sphere>();
  s1->set_transform(CreateTranslation(-2, 0, 0));
  auto s2 = std::make_shared<Sphere>();
  s2->set_transform(CreateTranslation(2, 0, 0));
  auto s3 = std::make_shared<Sphere>();
  auto g = std::make_shared<Group>();

  g->add(s1);
  g->add(s2);
  g->add(s3);

  std::pair<std::vector<std::shared_ptr<Shape>>, std::vector<std::shared_ptr<Shape>>> left_right = g->partition_children();

  ASSERT_EQ(1, g->children().size());
  EXPECT_EQ(s3, g->child<Sphere>(0));

  ASSERT_EQ(1, left_right.first.size());
  ASSERT_EQ(1, left_right.second.size());

  EXPECT_EQ(s1, left_right.first[0]);
  EXPECT_EQ(s2, left_right.second[0]);
}