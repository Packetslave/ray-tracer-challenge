//
// Created by Brian Landers on 2019-01-11.
//

#include "../core/canvas.h"
#include "../core/tuple.h"
#include "../shapes/sphere.h"

int main() {
  int canvas_pixels = 100;
  Canvas out(canvas_pixels, canvas_pixels);
  Color red(1, 0, 0);
  Color black(0, 0, 0);

  Sphere s;
  const auto origin = Tuple::point(0, 0, -5);
  int wall_z = 10;

  double wall_size = 7.0;
  double pixel_size = wall_size / canvas_pixels;
  double half = wall_size / 2;

  for (int y = 0; y < canvas_pixels - 1; ++y) {
    double world_y = half - pixel_size * y;

    for (int x = 0; x < canvas_pixels - 1; ++x) {
      double world_x = -half + pixel_size * x;

      const auto position = Tuple::point(world_x, world_y, wall_z);
      const auto direction = position - origin;

      Ray r(origin, Tuple::vector(direction).normalize());

      auto xs = s.intersects(r);
      if (Hit(xs)) {
        out.write_pixel(x, y, red);
      } else {
        out.write_pixel(x, y, black);
      }
    }
  }
  out.save("/tmp/raytrace1.ppm");
}