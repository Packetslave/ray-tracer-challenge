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
#include "../importers/yaml_file.h"
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

  auto world = World();
  auto file = read_file("/Users/blanders/tmp/spheres.yaml");

  auto parsed = std::make_shared<YamlFile>(file);
  auto parsed_group = parsed->to_group();
  auto parsed_camera = parsed->camera();
  auto parsed_light = parsed->light();

  world.set_light(parsed_light);
  world.add(parsed_group);
//
//  //auto ex = folly::ThreadedExecutor();
  auto ex = folly::CPUThreadPoolExecutor(20);
  auto task = parsed_camera->multi_render_sampled(world, 8);
  auto canvas = folly::coro::blockingWait(std::move(task).scheduleOn(&ex));
//  auto canvas = camera.render(world);
  canvas.save("/tmp/raytrace8.ppm");
}