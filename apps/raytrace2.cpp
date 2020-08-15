//
// Created by Brian Landers on 2019-01-11.
//

#include "../core/canvas.h"
#include "../core/light.h"
#include "../core/material.h"
#include "../core/tuple.h"
#include "../shapes/sphere.h"
#include "../utils/timer.h"

int main() {
  Timer t("Total Runtime");

  int canvas_pixels = 400;
  Canvas out(canvas_pixels, canvas_pixels);
  Color red(1, 0, 0);
  Color black(0, 0, 0);

  Sphere s;
  s.set_material(Material());

  auto light_position = Tuple::point(-10, 10, -10);
  auto light_color = Color(1, 1, 1);
  auto light = PointLight(light_position, light_color);

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
      auto hit = Hit(xs);

      if (hit) {
        auto point = r.position(hit->t());
        auto normal = hit->object()->normal_at(point);
        auto eye = -r.direction();
        auto color = hit->object()->material()->lighting(&s, light, point, eye,
                                                         normal, 1.0);

        out.write_pixel(x, y, color);
      } else {
        out.write_pixel(x, y, black);
      }
    }
  }
  out.save("/tmp/raytrace2.ppm");
}