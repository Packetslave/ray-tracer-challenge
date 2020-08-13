//
// Created by Brian Landers on 8/12/20.
//

#ifndef RAY_TRACING2_PBRT_FILE_H
#define RAY_TRACING2_PBRT_FILE_H

#include "../core/camera.h"
#include "../core/light.h"
#include "../core/matrix.h"
#include "../external/minipbrt/minipbrt.h"
#include "../shapes/shape.h"
#include "../shapes/sphere.h"
#include "file.h"

namespace {
double degrees_to_radians(double degrees) { return (degrees * PI) / 180; }

Matrix array_to_matrix(double p_ctrans[][4]) {
  Matrix ctrans;
  ctrans.set(0, 0, p_ctrans[0][0]);
  ctrans.set(0, 1, p_ctrans[0][1]);
  ctrans.set(0, 2, p_ctrans[0][2]);
  ctrans.set(0, 3, p_ctrans[0][3]);
  ctrans.set(1, 0, p_ctrans[1][0]);
  ctrans.set(1, 1, p_ctrans[1][1]);
  ctrans.set(1, 2, p_ctrans[1][2]);
  ctrans.set(1, 3, p_ctrans[1][3]);
  ctrans.set(2, 0, p_ctrans[2][0]);
  ctrans.set(2, 1, p_ctrans[2][1]);
  ctrans.set(2, 2, p_ctrans[2][2]);
  ctrans.set(2, 3, p_ctrans[2][3]);
  ctrans.set(3, 0, p_ctrans[3][0]);
  ctrans.set(3, 1, p_ctrans[3][1]);
  ctrans.set(3, 2, p_ctrans[3][2]);
  ctrans.set(3, 3, p_ctrans[3][3]);
  return ctrans;
}

Matrix lookat_to_vt(const Matrix& in) {
  // clang-format off
  auto lat = in.inverse();
  auto out = Matrix{MatrixData4{
      {
          { -1 * lat.get(0, 0), -1 * lat.get(0, 1), -1 * lat.get(0, 2), -1 * lat.get(0, 3) },
          {  1 * lat.get(1, 0),  1 * lat.get(1, 1),  1 * lat.get(1, 2),  1 * lat.get(1, 3) },
          { -1 * lat.get(2, 0), -1 * lat.get(2, 1), -1 * lat.get(2, 2), -1 * lat.get(2, 3) },
          {  1 * lat.get(3, 0),  1 * lat.get(3, 1),  1 * lat.get(3, 2),  1 * lat.get(3, 3) },
      }
  }};
  // clang-format on
  return out;
}
}  // namespace

class PBRTFile : public File {
 public:
  explicit PBRTFile(const std::string& blob, bool normalize = false)
      : File(blob, normalize) {
    std::string filename(blob);

    minipbrt::Loader loader;
    if (!loader.load(filename.c_str())) {
      const minipbrt::Error* err = loader.error();
      throw std::runtime_error(err->message());
    }

    std::unique_ptr<minipbrt::Scene> scene;
    scene.reset(loader.take_scene());

    int image_w, image_h;
    scene->film->get_resolution(image_w, image_h);

    auto p_camera = (minipbrt::PerspectiveCamera*)scene->camera;
    camera_ = new Camera(image_w, image_h, degrees_to_radians(p_camera->fov));

    auto look_at = array_to_matrix(scene->camera->cameraToWorld.start);
    camera_->set_transform(lookat_to_vt(look_at));

    auto p_light = (minipbrt::PointLight*)scene->lights[0];
    auto p_from = p_light->from;
    auto p_color = p_light->I;

    light_ = new PointLight(Tuple::point(p_from[0], p_from[1], p_from[2]),
                            Color(p_color[0], p_color[1], p_color[2]));

    for (const auto& s : scene->shapes) {
      std::shared_ptr<Shape> shape;

      switch (s->type()) {
        case minipbrt::ShapeType::Cone:
          break;
        case minipbrt::ShapeType::Curve:
          break;
        case minipbrt::ShapeType::Cylinder:
          break;
        case minipbrt::ShapeType::Disk:
          break;
        case minipbrt::ShapeType::Hyperboloid:
          break;
        case minipbrt::ShapeType::Paraboloid:
          break;
        case minipbrt::ShapeType::Sphere:
          shape = std::make_shared<Sphere>();
          break;
        case minipbrt::ShapeType::TriangleMesh:
          break;
        case minipbrt::ShapeType::HeightField:
          break;
        case minipbrt::ShapeType::LoopSubdiv:
          break;
        case minipbrt::ShapeType::Nurbs:
          break;
        case minipbrt::ShapeType::PLYMesh:
          break;
      }
      if (shape) {
        shape->set_transform(array_to_matrix(s->shapeToWorld.start));
        default_group_->add(shape);
      }
    }
  }

  PointLight* light() const override { return light_; }

  Camera* camera() const override { return camera_; }

 protected:
  PointLight* light_;
  Camera* camera_;
};

#endif  // RAY_TRACING2_PBRT_FILE_H
