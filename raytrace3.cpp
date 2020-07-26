//
// Created by Brian Landers on 2019-01-11.
//

#include "camera.h"
#include "canvas.h"
#include "light.h"
#include "material.h"
#include "sphere.h"
#include "timer.h"
#include "tuple.h"
#include "world.h"

int main() {
  Timer t("Total Time");

  auto world = World();
  world.set_light(PointLight(Tuple::point(-10, 10, -10), Color(1, 1, 1)));

  auto camera = Camera(800, 400, PI / 3);
  camera.set_transform(view_transform(
      Tuple::point(0, 1.5, -5), Tuple::point(0, 1, 0), Tuple::vector(0, 1, 0)));

  auto floor = Sphere();
  floor.set_transform(CreateScaling(10, 0.1, 10));
  auto mf = Material();
  mf.set_color(Color(1, 0.9, 0.9));
  mf.set_specular(0);
  floor.set_material(mf);
  world.add(floor);

  auto left_wall = Sphere();
  left_wall.set_transform(CreateTranslation(0, 0, 5) *
                          CreateRotationY(-PI / 4) * CreateRotationX(PI / 2) *
                          CreateScaling(10, 0.01, 10));
  left_wall.set_material(mf);
  world.add(left_wall);

  auto right_wall = Sphere();
  right_wall.set_transform(CreateTranslation(0, 0, 5) *
                           CreateRotationY(PI / 4) * CreateRotationX(PI / 2) *
                           CreateScaling(10, 0.01, 10));
  right_wall.set_material(mf);
  world.add(right_wall);

  auto middle = Sphere();
  middle.set_transform(CreateTranslation(-0.5, 1, 0.5));
  auto mm = Material();
  mm.set_color(Color(0.1, 1, 0.5));
  mm.set_diffuse(0.7);
  mm.set_specular(0.3);
  middle.set_material(mm);
  world.add(middle);

  auto right = Sphere();
  right.set_transform(CreateTranslation(1.5, 0.5, -0.5) *
                      CreateScaling(0.5, 0.5, 0.5));
  auto mr = Material();
  mr.set_color(Color(0.5, 1, 0.1));
  mr.set_diffuse(0.7);
  mr.set_specular(0.3);
  right.set_material(mr);
  world.add(right);

  auto left = Sphere();
  left.set_transform(CreateTranslation(-1.5, 0.33, -0.75) *
                     CreateScaling(0.33, 0.33, 0.33));
  auto ml = Material();
  ml.set_color(Color(1, 0.8, 0.1));
  ml.set_diffuse(0.7);
  ml.set_specular(0.3);
  left.set_material(ml);
  world.add(left);

  auto canvas = camera.render(world);
  canvas.save("/tmp/raytrace3.ppm");
}