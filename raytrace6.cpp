//
// Created by Brian Landers on 2019-01-11.
//

#include "camera.h"
#include "canvas.h"
#include "cube.h"
#include "light.h"
#include "material.h"
#include "matrix.h"
#include "plane.h"
#include "sphere.h"
#include "timer.h"
#include "tuple.h"
#include "world.h"

int main() {
  Timer t("Total Time");

  Camera camera{1600, 800, PI_4};
  camera.set_transform(view_transform(Tuple::point(8, 6, -8), Tuple::point(0, 3, 0), Tuple::vector(0, 1, 0)));
  PointLight light{Tuple::point(0, 6.9, -5), Color(1, 1, 0.9)};

  World w;
  w.set_light(light);

  // FLOOR / CEILING
  std::shared_ptr<Shape> floor_ceiling;
  floor_ceiling.reset(new Cube());

  floor_ceiling->set_transform(CreateScaling(20, 7, 20) * CreateTranslation(0, 1, 0));
  auto floor_ceiling_pattern = CheckPattern(Color(0, 0, 0),
                                                                Color(0.25, 0.25, 0.25));
  floor_ceiling_pattern.set_transform(CreateScaling(0.07, 0.07, 0.07));
  auto floor_ceiling_material = Material();
  floor_ceiling_material.set_pattern(floor_ceiling_pattern);
  floor_ceiling_material.set_ambient(0.25);
  floor_ceiling_material.set_diffuse(0.7);
  floor_ceiling_material.set_specular(0.9);
  floor_ceiling_material.set_shininess(300);
  floor_ceiling_material.set_reflective(0.1);
  floor_ceiling->set_material(floor_ceiling_material);
  w.add(floor_ceiling);

//
  // WALLS
  std::shared_ptr<Shape> walls;
  walls.reset(new Cube());

  walls->set_transform(CreateScaling(10, 10, 10));
  auto wall_pattern = CheckPattern(Color(0.4863, 0.3765, 0.2941),
                                                       Color(0.3725, 0.2902, 0.2275));
  wall_pattern.set_transform(CreateScaling(0.05, 20, 0.05));
  auto wall_material = Material();
  wall_material.set_pattern(wall_pattern);
  wall_material.set_ambient(0.1);
  wall_material.set_diffuse((0.7));
  wall_material.set_specular(0.9);
  wall_material.set_shininess(300);
  wall_material.set_reflective(0.05);
  walls->set_material(wall_material);
  w.add(walls);
//
  // TABLETOP
  std::shared_ptr<Shape> tabletop;
  tabletop.reset(new Cube());

  tabletop->set_transform(CreateTranslation(0, 3.1, 0) * CreateScaling(3, 0.1, 2));
  auto tabletop_pattern = GradientPattern(Color(1, 0, 0), Color(0, 0, 1));
  tabletop_pattern.set_transform(CreateScaling(0.05, 0.05, 0.05) * CreateRotationY(0.1));
  auto tabletop_material = Material();
  tabletop_material.set_pattern(tabletop_pattern);
  tabletop_material.set_ambient(0.1);
  tabletop_material.set_diffuse(0.7);
  tabletop_material.set_specular(0.9);
  tabletop_material.set_shininess(300);
  tabletop_material.set_reflective(0.2);
  tabletop->set_material(tabletop_material);
  w.add(tabletop);
//
//  // LEGS
  auto leg_material = Material();
  auto leg_pattern = SolidPattern(Color(0.5529, 0.4235, 0.3255));
  leg_material.set_pattern(leg_pattern);
  leg_material.set_ambient(0.2);
  leg_material.set_diffuse(0.7);

  std::shared_ptr<Shape> leg1;
  leg1.reset(new Cube());
  leg1->set_transform(CreateTranslation(2.7, 1.5, -1.7) * CreateScaling(0.1, 1.5, 0.1));
  leg1->set_material(leg_material);
  w.add(leg1);

  std::shared_ptr<Shape> leg2;
  leg2.reset(new Cube());
  leg2->set_transform(CreateTranslation(2.7, 1.5, 1.7) * CreateScaling(0.1, 1.5, 0.1));
  leg2->set_material(leg_material);
  w.add(leg2);

  std::shared_ptr<Shape> leg3;
  leg3.reset(new Cube());
  leg3->set_transform(CreateTranslation(-2.7, 1.5, -1.7) * CreateScaling(0.1, 1.5, 0.1));
  leg3->set_material(leg_material);
  w.add(leg3);

  std::shared_ptr<Shape> leg4;
  leg4.reset(new Cube());
  leg4->set_transform(CreateTranslation(-2.7, 1.5, 1.7) * CreateScaling(0.1, 1.5, 0.1));
  leg4->set_material(leg_material);
  w.add(leg4);

//  // GLASS CUBE
//  std::shared_ptr<Shape> glass_cube;
//  glass_cube.reset(new Cube());
//  glass_cube->set_transform(CreateTranslation(0, 3.45001, 0) * CreateRotationY(0.2) * CreateScaling(0.25, 0.25, 0.25));
//  //glass_cube->setCastsShadow(false);
//  auto glass_cube_material = Material();
//  auto glass_cube_pattern = SolidPattern(Color(1, 1, 0.8));
//  glass_cube_material.set_pattern(glass_cube_pattern);
//
//  glass_cube_material.set_ambient(0);
//  glass_cube_material.set_diffuse(0.3);
//  glass_cube_material.set_specular(0.9);
//  glass_cube_material.set_shininess(300);
//  glass_cube_material.set_reflective(0.7);
//  glass_cube_material.set_transparency(0.7);
//  glass_cube_material.set_refractive(1.5);
//  glass_cube->set_material(glass_cube_material);
//  w.add(glass_cube);

//  // LITTLE CUBE #1
//  auto little_cube1 = std::make_shared<Shape>(new Cube());
//  little_cube1->set_transform(CreateTranslation(1, 3.35, -0.9) * CreateRotationY(-0.4) * CreateScaling(0.15, 0.15, 0.15));
//  auto little_cube1_material = Material();std::make_shared<SolidPattern>(Color(1, 0.5, 0.5)));
//  little_cube1_material.set_reflective(0.6);
//  little_cube1_material.set_diffuse(0.4);
//  little_cube1->set_material(little_cube1_material);
//
//  // LITTLE CUBE #2
//  auto little_cube2 = std::make_shared<Shape>(new Cube());
//  little_cube2->set_transform(CreateTranslation(-1.5, 3.27, 0.3) * CreateRotationY(0.4) * CreateScaling(0.15, 0.07, 0.15));
//  auto little_cube2_material = Material();std::make_shared<SolidPattern>(Color(1, 1, 0.5)));
//  little_cube2->set_material(little_cube2_material);
//
//  // LITTLE CUBE #3
//  auto little_cube3 = std::make_shared<Shape>(new Cube());
//  little_cube3->set_transform(CreateTranslation(0, 3.25, 1) * CreateRotationY(0.4) * CreateScaling(0.2, 0.05, 0.05));
//  auto little_cube3_material = Material();std::make_shared<SolidPattern>(Color(0.5, 1, 0.5)));
//  little_cube3->set_material(little_cube3_material);
//
//  // LITTLE CUBE #4
//  auto little_cube4 = std::make_shared<Shape>(new Cube());
//  little_cube4->set_transform(CreateTranslation(-0.6, 3.4, -1) * CreateRotationY(0.8) * CreateScaling(0.05, 0.2, 0.05));
//  auto little_cube4_material = Material();std::make_shared<SolidPattern>(Color(0.5, 0.5, 1)));
//  little_cube4->set_material(little_cube4_material);
//
//  // LITTLE CUBE #5
//  auto little_cube5 = std::make_shared<Shape>(new Cube());
//  little_cube5->set_transform(CreateTranslation(2, 3.4, 1) * CreateRotationY(0.8) * CreateScaling(0.05, 0.2, 0.05));
//  auto little_cube5_material = Material();std::make_shared<SolidPattern>(Color(0.5, 1, 1)));
//  little_cube5->set_material(little_cube5_material);
//
//  // FRAME #1
//  auto frame1 = std::make_shared<Shape>(new Cube());
//  frame1->set_transform(CreateTranslation(-10, 4, 1) * CreateScaling(0.05, 1, 1));
//  auto frame1_material = Material();std::make_shared<SolidPattern>(Color(0.7098, 0.2471, 0.2196)));
//  frame1_material.set_diffuse(0.6);
//  frame1->set_material(frame1_material);
//
//  // FRAME #2
//  auto frame2 = std::make_shared<Shape>(new Cube());
//  frame2->set_transform(CreateTranslation(-10, 3.4, 2.7) * CreateScaling(0.05, 0.4, 0.4));
//  auto frame2_material = Material();std::make_shared<SolidPattern>(Color(0.2667, 0.2706, 0.602)));
//  frame2_material.set_diffuse(0.6);
//  frame2->set_material(frame2_material);
//
//  // FRAME #3
//  auto frame3 = std::make_shared<Shape>(new Cube());
//  frame3->set_transform(CreateTranslation(-10, 4.6, 2.7) * CreateScaling(0.05, 0.4, 0.4));
//  auto frame3_material = Material();std::make_shared<SolidPattern>(Color(0.3098, 0.5961, 0.3098)));
//  frame3_material.set_diffuse(0.6);
//  frame3->set_material(frame3_material);
//
//  // MIRROR FRAME
//  auto mirror_frame = std::make_shared<Shape>(new Cube());
//  mirror_frame->set_transform(CreateTranslation(-2, 3.5, 9.95) * CreateScaling(5, 1.5, 0.05));
//  auto mirror_frame_material = Material();std::make_shared<SolidPattern>(Color(0.3882, 0.2627, 0.1882)));
//  mirror_frame_material.set_diffuse(0.7);
//  mirror_frame->set_material(mirror_frame_material);
//
  // MIRROR
  std::shared_ptr<Shape> mirror;
  mirror.reset(new Cube());

  mirror->set_transform(CreateTranslation(-2, 3.5, 9.95) * CreateScaling(4.8, 1.4, 0.06));
  auto mirror_material = Material();std::make_shared<SolidPattern>(Color(0, 0, 0));
  mirror_material.set_diffuse(0);
  mirror_material.set_ambient(0);
  mirror_material.set_specular(1);
  mirror_material.set_shininess(300);
  mirror_material.set_reflective(1);
  mirror->set_material(mirror_material);
  w.add(mirror);

//  std::vector<std::shared_ptr<Shape>> shapes = {
//      floor_ceiling, walls,
//      tabletop, leg1, leg2, leg3, leg4,
//      glass_cube, little_cube1, little_cube2, little_cube3, little_cube4, little_cube5,
//      frame1, frame2, frame3, mirror_frame, mirror
//  };

  auto canvas = camera.render(w);
  canvas.save("/tmp/raytrace6.ppm");
}