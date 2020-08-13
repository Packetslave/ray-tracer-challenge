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
#include "../importers/pbrt_file.h"
#include "../shapes/cube.h"
#include "../shapes/plane.h"
#include "../shapes/sphere.h"
#include "../utils/timer.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/executors/ThreadedExecutor.h"
#include "folly/experimental/coro/BlockingWait.h"
#include "gflags/gflags.h"

DEFINE_int32(w, 1600, "image width");
DEFINE_int32(h, 1200, "image height");
DEFINE_bool(normalize_model, true, "normalize the model file on import");

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

void printMatrix(const Matrix& t) {
  std::cout << "[" << t.get(0, 0) << ", " << t.get(0, 1) << ", " << t.get(0, 2)
            << ", " << t.get(0, 3) << "]" << std::endl;
  std::cout << "[" << t.get(1, 0) << ", " << t.get(1, 1) << ", " << t.get(1, 2)
            << ", " << t.get(1, 3) << "]" << std::endl;
  std::cout << "[" << t.get(2, 0) << ", " << t.get(2, 1) << ", " << t.get(2, 2)
            << ", " << t.get(2, 3) << "]" << std::endl;
  std::cout << "[" << t.get(3, 0) << ", " << t.get(3, 1) << ", " << t.get(3, 2)
            << ", " << t.get(3, 3) << "]" << std::endl;
}

void printTuple(const float* t) {
  std::cout << "[" << t[0] << ", " << t[1] << ", " << t[2] << ", " << t[3]
            << "]" << std::endl;
}

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (argc < 2) {
    std::cerr << "ERROR: PBRT filename is required" << std::endl;
    exit(1);
  }
  std::string filename(argv[1]);

  std::unique_ptr<File> scene;
  std::shared_ptr<Group> root;

  std::unique_ptr<Camera> camera;
  std::unique_ptr<PointLight> light;

  {
    Timer t("Loading scene definition");
    if (filename.ends_with(".obj")) {
      camera = std::make_unique<Camera>(1600, 1200, PI_3);
      camera->set_transform(view_transform(Tuple::point(0, 1.5, -5),
                                          Tuple::point(0, 1, 0),
                                          Tuple::vector(0, 1, 0)));

      light = std::make_unique<PointLight>(Tuple::point(-10, 10, -10), Color(0.8, 0.8, 1));
      scene = std::make_unique<ObjFile>(filename, FLAGS_normalize_model);
    } else if (filename.ends_with(".pbrt")) {
      scene = std::make_unique<PBRTFile>(filename, FLAGS_normalize_model);
      light.reset(scene->light());
      camera.reset(scene->camera());
    } else {
      throw std::runtime_error("Unknown file type");
    }
    root = scene->to_group();
  }

  {
    Timer t("Optimizing model");
    root->divide(50);
    std::cout << "Size after divide(): " << root->size(/* recurse */ true)
              << std::endl;
  }

  std::unique_ptr<Canvas> canvas;
  {
    Timer t("Rendering");
    auto world = World();
    world.set_light(*light);
    world.add(root);

    auto ex = folly::CPUThreadPoolExecutor(20);
    auto task = camera->multi_render(world);
    canvas = std::make_unique<Canvas>(
        folly::coro::blockingWait(std::move(task).scheduleOn(&ex)));
  }
  canvas->save("/tmp/render.ppm");
}