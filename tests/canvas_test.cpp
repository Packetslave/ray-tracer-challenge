#include "../canvas.h"

#include "absl/strings/str_split.h"
#include "gtest/gtest.h"
#include "../color.h"


class CanvasTest : public ::testing::Test {
    
};

TEST(CanvasTest, Initialize) {
  // Scenario: Creating a canvas

  // Given c ← canvas(10, 20)
  auto c = Canvas(10, 20);

  // Then c.width = 10
  EXPECT_EQ(10, c.width());

  // And c.height = 20
  EXPECT_EQ(20, c.height());

  // And every pixel of c is color(0, 0, 0)
  for (int j = 0; j < 20; ++j) {
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(Color(0, 0, 0), c.pixel_at(j, i));
    }
  }
}

TEST(CanvasTest, SetPixels) {
  // Scenario: Writing pixels to a canvas

  // Given c ← canvas(10, 20)
  auto c = Canvas(10, 20);

  // And red ← color(1, 0, 0)
  auto red = Color(1, 0, 0);

  // When write_pixel(c, 2, 3, red)
  c.write_pixel(2, 3, red);

  // Then pixel_at(c, 2, 3) = red
  EXPECT_EQ(red, c.pixel_at(2, 3));
}

TEST(CanvasTest, Header) {
  // Scenario: Constructing the PPM header

  // Given c ← canvas(5, 3)
  auto c = Canvas(5, 3);

  // When ppm ← canvas_to_ppm(c)
  auto ppm = c.to_ppm();

  // Then lines 1-3 of ppm are """
  //     P3
  //     5 3
  //     255
  // """
  std::vector<std::string> lines = absl::StrSplit(ppm, "\n");
  EXPECT_GE(lines.size(), 3);
  EXPECT_EQ("P3", lines[0]);
  EXPECT_EQ("5 3", lines[1]);
  EXPECT_EQ("255", lines[2]);
}

TEST(CanvasTest, GeneratePPM) {
  // Scenario: Constructing the PPM pixel data

  // Given c ← canvas(5, 3)
  auto c = Canvas(5, 3);

  // And c1 ← color(1.5, 0, 0)
  auto c1 = Color(1.5, 0, 0);

  // And c2 ← color(0, 0.5, 0)
  auto c2 = Color(0, 0.5, 0);

  // And c3 ← color(-0.5, 0, 1)
  auto c3 = Color(-0.5, 0, 1);

  // When write_pixel(c, 0, 0, c1)
  c.write_pixel(0, 0, c1);

  // And write_pixel(c, 2, 1, c2)
  c.write_pixel(2, 1, c2);

  // And write_pixel(c, 4, 2, c3)
  c.write_pixel(4, 2, c3);

  // And ppm ← canvas_to_ppm(c)
  auto ppm = c.to_ppm();
  std::cout << ppm << std::endl;

  // Then lines 4-6 of ppm are
  //    """
  //    255 0 0 0 0 0 0 0 0 0 0 0 0 0 0
  //    0 0 0 0 0 0 0 128 0 0 0 0 0 0 0
  //    0 0 0 0 0 0 0 0 0 0 0 0 0 0 255
  //    """
  std::vector<std::string> lines = absl::StrSplit(ppm, "\n");
  EXPECT_EQ("255 0 0 0 0 0 0 0 0 0 0 0 0 0 0", lines[3]);
  EXPECT_EQ("0 0 0 0 0 0 0 128 0 0 0 0 0 0 0", lines[4]);
  EXPECT_EQ("0 0 0 0 0 0 0 0 0 0 0 0 0 0 255", lines[5]);
}

TEST(CanvasTest, SplitLongLines) {
  // Scenario: Splitting long lines in PPM files

  // Given c ← canvas(10, 2)
  auto c = Canvas(10, 2);

  // When every pixel of c is set to color(1, 0.8, 0.6)
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 10; ++x) {
      c.write_pixel(x, y, Color(1, 0.8, 0.6));
    }
  }

  // And ppm ← canvas_to_ppm(c)
  auto ppm = c.to_ppm();

  // Then lines 4-7 of ppm are
  //     """
  //         255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204
  //         153 255 204 153 255 204 153 255 204 153 255 204 153
  //         255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204
  //         153 255 204 153 255 204 153 255 204 153 255 204 153
  //     """
  std::vector<std::string> lines = absl::StrSplit(ppm, "\n");
  EXPECT_EQ(
      "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204",
      lines[3]);
  EXPECT_EQ("153 255 204 153 255 204 153 255 204 153 255 204 153", lines[4]);
  EXPECT_EQ(
      "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204",
      lines[5]);
  EXPECT_EQ("153 255 204 153 255 204 153 255 204 153 255 204 153", lines[6]);
}

TEST(CanvasTest, EndWithNewline) {
  // Scenario: PPM files are terminated by a newline character

  // Given c ← canvas(5, 3)
  auto c = Canvas(5, 3);

  // When ppm ← canvas_to_ppm(c)
  auto ppm = c.to_ppm();

  // Then ppm ends with a newline character
  EXPECT_EQ('\n', ppm.at(ppm.size() - 1));
}