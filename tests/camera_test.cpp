#include "../core/camera.h"

#include <cmath>

#include "../core/world.h"
#include "gtest/gtest.h"
#include "test_common.h"

TEST(Camera, Create) {
  auto c = Camera(160, 120, PI_2);
  EXPECT_EQ(160, c.hsize());
  EXPECT_EQ(120, c.vsize());
  EXPECT_NEAR(PI_2, c.field_of_view(), EPSILON);
}

TEST(Camera, PixelSizeH) {
  auto c = Camera(200, 125, PI_2);
  EXPECT_NEAR(c.pixel_size(), 0.01, EPSILON);
}

TEST(Camera, PixelSizeV) {
  auto c = Camera(125, 200, PI_2);
  auto actual = c.pixel_size();
  auto expected = 0.01;
  EXPECT_NEAR(actual, expected, EPSILON);
}

TEST(Camera, RayThroughCenter) {
  auto c = Camera(201.0, 101.0, PI_2);
  auto r = c.ray_for_pixel(100.0, 50.0);

  auto expectedp = Tuple::point(0.0, 0.0, 0.0);
  auto actualp = r.origin();
  EXPECT_TRUE(tuple_is_near(expectedp, actualp));

  auto expectedv = Tuple::vector(0, 0, -1);
  auto actualv = r.direction();
  EXPECT_TRUE(tuple_is_near(expectedv, actualv));
}

TEST(Camera, RayThroughCorner) {
  auto c = Camera(201, 101, PI_2);
  auto r = c.ray_for_pixel(0, 0);

  auto expectedp = Tuple::point(0, 0, 0);
  EXPECT_TRUE(tuple_is_near(expectedp, r.origin()));

  auto expectedv = Tuple::vector(0.66519, 0.33259, -0.66851);
  EXPECT_TRUE(tuple_is_near(expectedv, r.direction()));
}

TEST(Camera, RayWithTransformedCamera) {
  auto c = Camera(201, 101, PI_2);
  Matrix t = CreateRotationY(PI_4) * CreateTranslation(0, -2, 5);
  c.set_transform(t);

  auto r = c.ray_for_pixel(100.0, 50.0);

  auto expectedp = Tuple::point(0.0, 2.0, -5.0);
  EXPECT_TRUE(tuple_is_near(expectedp, r.origin())) << r.origin() << std::endl
                                                    << expectedp;

  auto expectedv = Tuple::vector(SQRT2_2, 0, -SQRT2_2);
  EXPECT_TRUE(tuple_is_near(expectedv, r.direction()));
}

TEST(Camera, RenderWorld) {
  auto w = World::default_world();
  auto c = Camera(11, 11, PI_2);
  auto from = Tuple::point(0, 0, -5);
  auto to = Tuple::point(0, 0, 0);
  auto up = Tuple::vector(0, 1, 0);
  auto vt = view_transform(from, to, up);
  c.set_transform(vt);
  auto image = c.multi_render_tbb(w, 1);
  auto expected = Color(0.38066, 0.47583, 0.2855);
  auto actual = image.pixel_at(5, 5);
  EXPECT_TRUE(tuple_is_near(expected, actual)) << expected << " != " << actual;
}
