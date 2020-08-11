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
  Timer t("Total Time");

  auto camera = Camera(1600, 1200, PI_3);
  camera.set_transform(view_transform(
      Tuple::point(0, 1.5, -5), Tuple::point(0, 1, 0), Tuple::vector(0, 1, 0)));

  auto world = World();
  world.set_light(PointLight(Tuple::point(-10, 10, -10), Color(1, 1, 1)));

  auto file = read_file("/Users/blanders/tmp/astronaut1.obj");
  std::shared_ptr<Group> group;
  std::shared_ptr<ObjFile> parsed;
  {
    Timer t2("Building model");
    parsed = std::make_shared<ObjFile>(file, true);
    group = parsed->to_group();
    //group->set_transform(CreateRotationY(-PI));
  }

  {
    Timer t3("Optimizing model");
    //group->divide(100);
    std::cout << "Size after divide(): " << group->size(/* recurse */ true) << std::endl;
  }

  world.add(group);

  //auto ex = folly::ThreadedExecutor();
  auto ex = folly::CPUThreadPoolExecutor(20);
  auto task = camera.multi_render(world);
  auto canvas = folly::coro::blockingWait(std::move(task).scheduleOn(&ex));
  //auto canvas = camera.render(world);
  canvas.save("/tmp/raytrace7.ppm");
}