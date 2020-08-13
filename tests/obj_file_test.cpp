//
// Created by Brian Landers on 8/8/20.
//

#include "../importers/obj_file.h"

#include "../core/tuple.h"
#include "../shapes/triangle.h"
#include "gtest/gtest.h"
#include "../importers/obj_file.h"

TEST(ObjectFile, Unrecognized) {
  std::string gibberish = {
      "There was a young lady named Bright\n"
      "who traveled much faster than light.\n"
      "She set out one day\n"
      "in a relative way,\n"
      "and came back the previous night.\n"
  };
  auto parsed = ObjFile(gibberish);
  EXPECT_EQ(5, parsed.ignored());
}

TEST(ObjectFile, VertexData) {
  std::string file = {
      "v -1 1 0\n"
      "v -1.0000 0.5000 0.0000\n"
      "v 1 0 0\n"
      "v 1 1 0\n"
  };
  auto parsed = ObjFile(file);
  EXPECT_EQ(5, parsed.vertices().size());
  EXPECT_EQ(Tuple::point(-1, 1, 0), parsed.vertices()[1]);
  EXPECT_EQ(Tuple::point(-1, 0.5, 0), parsed.vertices()[2]);
  EXPECT_EQ(Tuple::point(1, 0, 0), parsed.vertices()[3]);
  EXPECT_EQ(Tuple::point(1, 1, 0), parsed.vertices()[4]);
}

TEST(ObjectFile, TriangleData) {
  std::string file = {
      "v -1 1 0\n"
      "v -1 0 0\n"
      "v 1 0 0 \n"
      "v 1 1 0\n"
      "f 1 2 3\n"
      "f 1 3 4\n"
  };
  auto parsed = ObjFile(file);
  auto g = parsed.default_group();
  auto t1 = g->child<Triangle>(0);
  auto t2 = g->child<Triangle>(1);
  EXPECT_EQ(parsed.vertices()[1], t1->p1);
  EXPECT_EQ(parsed.vertices()[2], t1->p2);
  EXPECT_EQ(parsed.vertices()[3], t1->p3);
  EXPECT_EQ(parsed.vertices()[1], t2->p1);
}

TEST(ObjectFile, PolygonData) {
  std::string file = {
      "v -1 1 0\n"
      "v -1 0 0\n"
      "v 1 0 0\n"
      "v 1 1 0\n"
      "v 0 2 0\n"
      "f 1 2 3 4 5\n"
  };
  auto parsed = ObjFile(file);
  auto g = parsed.default_group();
  auto t1 = g->child<Triangle>(0);
  auto t2 = g->child<Triangle>(1);
  auto t3 = g->child<Triangle>(2);
  EXPECT_EQ(t1->p1 , parsed.vertices()[1]);
  EXPECT_EQ(t1->p2 , parsed.vertices()[2]);
  EXPECT_EQ(t1->p3 , parsed.vertices()[3]);
  EXPECT_EQ(t2->p1 , parsed.vertices()[1]);
  EXPECT_EQ(t2->p2 , parsed.vertices()[3]);
  EXPECT_EQ(t2->p3 , parsed.vertices()[4]);
  EXPECT_EQ(t3->p1 , parsed.vertices()[1]);
  EXPECT_EQ(t3->p2 , parsed.vertices()[4]);
  EXPECT_EQ(t3->p3 , parsed.vertices()[5]);
}

TEST(ObjectFile, DISABLED_NamedGroup) {
  std::string file = {
      "v -1 1 0\n"
      "v -1 0 0\n"
      "v 1 0 0\n"
      "v 1 1 0\n"
      "\n"
      "g FirstGroup\n"
      "f 1 2 3\n"
      "\n"
      "g SecondGroup\n"
      "f 1 3 4\n"
  };
  auto parsed = ObjFile(file);
  auto g1 = parsed.group("FirstGroup");
  auto g2 = parsed.group("SecondGroup");
  auto t1 = g1->child<Triangle>(0);
  auto t2 = g2->child<Triangle>(0);
  EXPECT_EQ(parsed.vertices()[1], t1->p1);
  EXPECT_EQ(parsed.vertices()[2], t1->p2);
  EXPECT_EQ(parsed.vertices()[3], t1->p3);
  EXPECT_EQ(parsed.vertices()[1], t2->p1);
  EXPECT_EQ(parsed.vertices()[3], t2->p2);
  EXPECT_EQ(parsed.vertices()[4], t2->p3);
}

TEST(ObjectFile, ToGroup) {
  std::string file = {
      "v -1 1 0\n"
      "v -1 0 0\n"
      "v 1 0 0\n"
      "v 1 1 0\n"
      "\n"
      "g FirstGroup\n"
      "f 1 2 3\n"
      "\n"
      "g SecondGroup\n"
      "f 1 3 4\n"
  };
  auto parsed = ObjFile(file);
  auto g = parsed.to_group();
}

TEST(ObjectFile, Normals) {
  std::string file = {
    "vn 0 0 1\n"
    "vn 0.707 0 -0.707\n"
    "vn 1 2 3\n",
  };
  auto parsed = ObjFile(file);
  EXPECT_EQ(Tuple::vector(0, 0, 1), parsed.normals()[1]);
  EXPECT_EQ(Tuple::vector(0.707, 0, -0.707), parsed.normals()[2]);
  EXPECT_EQ(Tuple::vector(1, 2, 3), parsed.normals()[3]);
}

TEST(ObjectFile, FacesWithNormals) {
  std::string file = {
      "v 0 1 0\n"
      "v -1 0 0\n"
      "v 1 0 0\n"
      "\n"
      "vn -1 0 0\n"
      "vn 1 0 0\n"
      "vn 0 1 0\n"
      "\n"
      "f 1//3 2//1 3//2\n"
      "f 1/0/3 2/102/1 3/14/2\n"
  };
  auto parsed = ObjFile(file);
  auto g = parsed.to_group();
  ASSERT_EQ(2, g->children().size());

  auto t1 = g->child<SmoothTriangle>(0);
  auto t2 = g->child<SmoothTriangle>(1);
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);

  auto v = parsed.vertices();
  auto n = parsed.normals();
  EXPECT_EQ(v[1], t1->p1);
  EXPECT_EQ(v[2], t1->p2);
  EXPECT_EQ(v[3], t1->p3);
  EXPECT_EQ(n[3], t1->n1);
  EXPECT_EQ(n[1], t1->n2);
  EXPECT_EQ(n[2], t1->n3);
  EXPECT_EQ(*t1, *t2);
}