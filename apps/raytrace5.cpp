//
// Created by Brian Landers on 2019-01-11.
//

#include "../core/camera.h"
#include "../core/canvas.h"
#include "../core/light.h"
#include "../core/material.h"
#include "../core/matrix.h"
#include "../core/tuple.h"
#include "../core/world.h"
#include "../shapes/plane.h"
#include "../shapes/sphere.h"
#include "../utils/timer.h"

int main() {
  Timer t("Total Time");

  auto world = World();
  world.set_light(PointLight(Tuple::point(-10, 10, -10), Color(1, 1, 1)));

  auto camera = Camera(1600, 1200, PI_3);
  camera.set_transform(view_transform(
      Tuple::point(0, 1.5, -5), Tuple::point(0, 1, 0), Tuple::vector(0, 1, 0)));

  std::shared_ptr<Shape> floor;
  floor.reset(new Plane());

  floor->set_transform(CreateScaling(10, 0.1, 10));
  auto mf = Material();
  mf.set_color(Color(1, 0.9, 0.9));
  mf.set_specular(0.4);
  mf.set_reflective(0.9);

  auto pattern = StripePattern(Color(1, 1, 1), Color(0, 0, 0));
  mf.set_pattern(pattern);

  floor->set_material(mf);
  world.add(floor);

  std::shared_ptr<Shape> middle;
  middle.reset(new Sphere());

  middle->set_transform(CreateTranslation(-0.5, 1, 0.5));
  auto mm = Material();
  mm.set_color(Color(0.1, 1, 1.0));
  middle->set_material(mm);
  world.add(middle);

  std::shared_ptr<Shape> right;
  right.reset(new Sphere());

  right->set_transform(CreateTranslation(1.5, 0.5, -0.5) *
                       CreateScaling(0.5, 0.5, 0.5));
  auto mr = Material();
  mr.set_color(Color(0.5, 1, 0.1));
  mr.set_diffuse(0.7);
  mr.set_specular(0.3);
  right->set_material(mr);
  world.add(right);

  std::shared_ptr<Shape> left;
  left.reset(new Sphere());

  left->set_transform(CreateTranslation(-1.5, 0.33, -0.75) *
                      CreateScaling(0.33, 0.33, 0.33));
  auto ml = Material();
  ml.set_color(Color(1, 0.8, 0.1));
  ml.set_diffuse(0.7);
  ml.set_specular(0.3);
  left->set_material(ml);
  world.add(left);

  auto canvas = camera.render(world);
  canvas.save("/tmp/raytrace5.ppm");
}