#include "../light.h"

#include "../color.h"
#include "../tuple.h"
#include "gtest/gtest.h"

TEST(Light, PointLight) {
  auto i = Color(1, 1, 1);
  auto p = Tuple::point(0, 0, 0);
  auto l = PointLight(p, i);
  EXPECT_EQ(p, l.position());
  EXPECT_EQ(i, l.intensity());
}
