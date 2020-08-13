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
#include "../importers/obj_file.h"
#include "../shapes/cube.h"
#include "../shapes/plane.h"
#include "../shapes/sphere.h"
#include "../utils/timer.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/executors/ThreadedExecutor.h"
#include "folly/experimental/coro/BlockingWait.h"

auto read_file(std::string_view path) -> std::string {
  constexpr auto read_size = std::size_t{4096};
  auto stream = std::ifstream{path.data()};
  stream.exceptions(std::ios_base::badbit);

  auto out = std::string{};
  auto buf = std::string(read_size, '\0');
  while (stream.read(&buf[0], read_size)) {
    out.append(buf, 0, stream.gcount());
  }
  out.append(buf, 0, stream.gcount());
  return out;
}

int main() {
  auto camera = Camera(100, 50, PI_3);
  camera.set_transform(view_transform(
      Tuple::point(5, 10, -40), Tuple::point(0, 15, 0), Tuple::vector(0, 1, 0)));

  auto world = World();
  world.set_light(PointLight(Tuple::point(-20, 20, -20), Color(0.8, 0.8, 0.8)));

  auto floor = std::make_shared<Plane>();
  floor->set_transform(CreateScaling(10, -0.01, 10));
  auto mf = Material();
  mf.set_color(Color(0.9, 0, 0));
  mf.set_specular(0.5);
  mf.set_reflective(0.25);
  floor->set_material(mf);
  world.add(floor);

  auto file = read_file("/Users/blanders/Downloads/teapot.obj");
  std::shared_ptr<Group> group;
  std::shared_ptr<ObjFile> parsed;


    auto t = Timer("Building model");
    parsed = std::make_shared<ObjFile>(file, false);
    group = parsed->to_group();
    // group->set_transform(CreateRotationX(-PI_2) * CreateTranslation(0, 0,
    // 0.5));
    group->set_transform(CreateTranslation(0, 20, 0) * CreateRotationY(-PI));
    t.stop();

  t = Timer("Optimizing model");
    group->divide(50);
    std::cout << "Size after divide(): " << group->size(/* recurse */ true)
              << std::endl;
  t.stop();

  world.add(group);

  // auto ex = folly::ThreadedExecutor();
//  auto ex = folly::CPUThreadPoolExecutor(20);
  auto canvas = camera.multi_render_tbb(world, 4);
  //auto canvas = folly::coro::blockingWait(std::move(task).scheduleOn(&ex));
  // auto canvas = camera.render(world);
  t = Timer("Saving to disk");
  canvas.save("/tmp/raytrace7.ppm");
  t.stop();
}